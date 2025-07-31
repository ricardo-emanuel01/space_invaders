# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include "raylib.h"


# define SCREEN_WIDTH 1920
# define SCREEN_HEIGHT 1080
# define SHIP_WIDTH 100
# define SHIP_HEIGHT 50
# define SHIP_POS_Y 900
# define ENEMY_SHIP_WIDTH SHIP_WIDTH
# define ENEMY_SHIP_HEIGHT SHIP_HEIGHT
# define ENEMY_SHIP_POS_Y 50
# define ENEMIES_GAP_X 25
# define ENEMIES_GAP_Y 15
# define ENEMIES_WIDTH 40
# define ENEMIES_HEIGHT ENEMIES_WIDTH
# define ENEMIES_OFFSET_Y (ENEMIES_WIDTH*3)
# define N_ENEMIES_ROWS 4
# define ENEMIES_PER_ROW 11
# define N_ENEMIES (N_ENEMIES_ROWS*ENEMIES_PER_ROW)
# define EXIT_BANNER_HEIGHT 200
# define EXIT_MESSAGE_FONT_SIZE 75
# define BULLET_SPEED 20 // pixels/frame
# define N_BULLETS 10000
# define BULLET_WIDTH 5
# define BULLET_HEIGHT 40
# define ENTITIES_ARRAY_SIZE (2+N_ENEMIES+N_BULLETS)
# define FIRST_IDX_BULLETS (2+N_ENEMIES)
# define SCREEN_LIMIT 250
# define SCREEN_LIMIT_LEFT SCREEN_LIMIT
# define SCREEN_LIMIT_RIGHT (SCREEN_WIDTH-SCREEN_LIMIT)
# define HORDE_SCREEN_LIMIT 400
# define HORDE_LIMIT_LEFT HORDE_SCREEN_LIMIT
# define HORDE_LIMIT_RIGHT (SCREEN_WIDTH-HORDE_SCREEN_LIMIT)
# define WALL_COLISION_DELAY 1.0f


const char exitMessage[] = "Are you sure you want to exit game? [Y/N]";

typedef enum GameState {
    MENU,
    PLAYING,
    WIN,
    LOSE,
    PAUSED,
} GameState;

typedef enum EntityType {
    SHIP,
    ENEMY_SHIP,
    ENEMY_SLOW,
    ENEMY_FAST,
    BULLET,
} EntityType;

typedef struct Entity {
    EntityType type;
    Vector2 size;
    Vector2 position;
    Vector2 velocity;
    Color color;
    bool alive;
    float delayToFire;
    bool canFire;
    double lastShotTime;
    EntityType shotSrc;
    int points;
} Entity;

typedef struct GameData {
    bool exitWindow;
    bool canCollideOnWall;
    double timeLastWallCollision;
    float wallCollisionDelay;
    float incrementPerLevel;
    int firstAlive;
    int score;
    Entity *entities;
    GameState gameState;
    Music BGMusic;
    Sound shipFire;
    Sound alienFire;
    Sound shipExplosion;
    Sound alienExplosion;
} GameData;

Entity *buildEntities(const Color colors[], const float delayToFire[]) {
    Entity *entities = (Entity *)malloc((ENTITIES_ARRAY_SIZE) * sizeof(Entity));
    // Setup player ship
    entities[SHIP].type = SHIP;
    entities[SHIP].size.x = SHIP_WIDTH;
    entities[SHIP].size.y = SHIP_HEIGHT;
    entities[SHIP].position.x = (SCREEN_WIDTH - SHIP_WIDTH)/2;
    entities[SHIP].position.y = SHIP_POS_Y;
    entities[SHIP].velocity.x = 0.0f;
    entities[SHIP].velocity.y = 0.0f;
    entities[SHIP].color = colors[0];
    entities[SHIP].alive = true;
    entities[SHIP].delayToFire = delayToFire[SHIP];
    entities[SHIP].canFire = true;
    entities[SHIP].lastShotTime = GetTime();

    int enemiesXOffSet = (SCREEN_WIDTH - ENEMIES_PER_ROW*(ENEMIES_WIDTH + ENEMIES_GAP_X))/2;
    EntityType entityType = ENEMY_SHIP;
    for (int i = 0; i < N_ENEMIES; ++i) {
        if (i % (N_ENEMIES/2) == 0) entityType++;
        entities[i+2].type = entityType;
        entities[i+2].size.x = ENEMIES_WIDTH;
        entities[i+2].size.y = ENEMIES_WIDTH;
        entities[i+2].position.x = enemiesXOffSet + (ENEMIES_WIDTH + ENEMIES_GAP_X)*(i%ENEMIES_PER_ROW);
        entities[i+2].position.y = ENEMIES_OFFSET_Y + (ENEMIES_WIDTH + ENEMIES_GAP_Y)*(i/ENEMIES_PER_ROW);
        entities[i+2].velocity.x = 2.0f;
        entities[i+2].velocity.y = 0.0f;
        entities[i+2].color = colors[entityType];
        entities[i+2].alive = true;
        entities[i+2].delayToFire = delayToFire[entityType];
        entities[i+2].canFire = true;
        entities[i+2].lastShotTime = GetTime();
        if (entityType == ENEMY_FAST) {
            entities[i+2].points = 500;
        } else if (entityType == ENEMY_SLOW) {
            entities[i+2].points = 350;
        }
    }

    // Setup Enemy ship
    entities[ENEMY_SHIP].type = ENEMY_SHIP;
    entities[ENEMY_SHIP].size.x = ENEMY_SHIP_WIDTH;
    entities[ENEMY_SHIP].size.y = ENEMY_SHIP_HEIGHT;
    entities[ENEMY_SHIP].position.x = (SCREEN_WIDTH - ENEMY_SHIP_WIDTH)/2;
    entities[ENEMY_SHIP].position.y = ENEMY_SHIP_POS_Y;
    entities[ENEMY_SHIP].velocity.x = 5.0f;
    entities[ENEMY_SHIP].velocity.y = 0.0f;
    entities[ENEMY_SHIP].color = colors[ENEMY_SHIP];
    entities[ENEMY_SHIP].alive = false;
    entities[ENEMY_SHIP].delayToFire = delayToFire[ENEMY_SHIP];
    entities[ENEMY_SHIP].canFire = true;
    entities[ENEMY_SHIP].lastShotTime = GetTime();
    entities[ENEMY_SHIP].points = 5000;

    for (int i = FIRST_IDX_BULLETS; i < ENTITIES_ARRAY_SIZE; ++i) {
        entities[i].type = BULLET;
        entities[i].size.x = BULLET_WIDTH;
        entities[i].size.y = BULLET_HEIGHT;
        entities[i].alive = false;
        entities[i].color = RAYWHITE;
    }

    return entities;
}

GameData initGame() {
    srand(time(NULL));
    const Color colors[] = {RAYWHITE, YELLOW, DARKPURPLE, DARKGREEN};
    const float delayToFire[] = {0.5f, 3.0f, 1.0f, 0.1f};

    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Space Invaders clone using raylib"
    );
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    DisableCursor();
    GameData gameData = {
        .exitWindow=false,
        .canCollideOnWall=true,
        .timeLastWallCollision=0.0,
        .wallCollisionDelay=WALL_COLISION_DELAY,
        .incrementPerLevel=0.5f,
        .firstAlive=2,
        .entities=buildEntities(colors, delayToFire),
        .gameState=PLAYING,
        .score=0,
        .BGMusic=LoadMusicStream("resources/raining-bits.ogg"),
        .shipFire=LoadSound("resources/player-shoot.ogg"),
        .alienFire=LoadSound("resources/enemy-shoot.ogg"),
        .shipExplosion=LoadSound("resources/player-explosion.ogg"),
        .alienExplosion=LoadSound("resources/enemy-explosion.ogg"),
    };

    gameData.BGMusic.looping = true;

    PlayMusicStream(gameData.BGMusic);

    return gameData;
}

void closeGame(GameData *gameData) {
    UnloadMusicStream(gameData->BGMusic);
    UnloadSound(gameData->shipFire);
    UnloadSound(gameData->shipExplosion);
    UnloadSound(gameData->alienFire);
    UnloadSound(gameData->alienExplosion);
    CloseAudioDevice();
    CloseWindow();

    free(gameData->entities);
}

void drawExitMessage() {
    DrawRectangle(
        0,
        (SCREEN_HEIGHT - EXIT_BANNER_HEIGHT)/2,
        SCREEN_WIDTH,
        EXIT_BANNER_HEIGHT,
        RAYWHITE
    );
    DrawText(
        exitMessage,
        125,
        SCREEN_HEIGHT/2,
        EXIT_MESSAGE_FONT_SIZE,
        BLACK
    );
}

void drawEntities(Entity *entities) {
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        if (entities[i].alive)
            DrawRectangleV(
                entities[i].position,
                entities[i].size,
                entities[i].color
            );
    }
}

void drawGame(GameData *gameData) {
    ClearBackground(BLACK);
    DrawFPS(10, 10);

    switch (gameData->gameState) {
        // case MENU:
        // {

        // } break;
        case PLAYING:
        {
            drawEntities(gameData->entities);
        } break;
        case PAUSED:
        {
            drawEntities(gameData->entities);
            drawExitMessage();
        } break;
        // case WIN:
        // {

        // } break;
        // case LOSE:
        // {

        // } break;
        // While WIN and LOSE are not implemented
        default:
        {
            drawEntities(gameData->entities);
            drawExitMessage();
        } break;
    }
}

void debugEntities(Entity *entities) {
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        printf("Entity at index: %d\n", i);
        printf("\tType: %d\n", entities[i].type);
        printf("\tWidth: %lf\n", entities[i].size.x);
        printf("\tHeight: %lf\n", entities[i].size.y);
        printf("\tposX: %lf\n", entities[i].position.x);
        printf("\tposY: %lf\n", entities[i].position.y);
        printf("\tDelay to shoot: %.2lf\n", entities[i].delayToFire);
    }
}

void fire(GameData *gameData, int shooterIdx) {
    if (!gameData->entities[shooterIdx].canFire) return;

    Entity *entities = gameData->entities;
    // There are a small number of bullets, so for now a linear search is ok
    int firstBulletAvailable;
    for (firstBulletAvailable = FIRST_IDX_BULLETS; firstBulletAvailable < ENTITIES_ARRAY_SIZE && entities[firstBulletAvailable].alive; ++firstBulletAvailable);
    if (!entities[firstBulletAvailable].alive) {
        entities[shooterIdx].canFire = false;
        entities[shooterIdx].lastShotTime = GetTime();

        entities[firstBulletAvailable].position.x = entities[shooterIdx].position.x + 0.5*entities[shooterIdx].size.x;
        entities[firstBulletAvailable].alive = true;
        entities[firstBulletAvailable].shotSrc = entities[shooterIdx].type;

        if (entities[shooterIdx].type == SHIP) {
            entities[firstBulletAvailable].velocity.y = -BULLET_SPEED;
            entities[firstBulletAvailable].position.y = entities[shooterIdx].position.y;
            PlaySound(gameData->shipFire);
        } else {
            entities[firstBulletAvailable].velocity.y = BULLET_SPEED;
            entities[firstBulletAvailable].position.y = entities[shooterIdx].position.y + entities[shooterIdx].size.y;
            PlaySound(gameData->alienFire);
        }
    }
}


// Implements a naive collision detection
void detectCollisions(GameData *gameData) {
    Entity *entities = gameData->entities;
    for (int i = 0; i < FIRST_IDX_BULLETS; ++i) {
        if (!entities[i].alive) continue;

        Vector2 upperLeft = entities[i].position;
        Vector2 upperRight = {
            upperLeft.x + entities[i].size.x,
            upperLeft.y
        };
        Vector2 lowerLeft = {
            upperLeft.x,
            upperLeft.y + entities[i].size.y
        };
        Vector2 lowerRight = {
            lowerLeft.x + entities[i].size.x,
            lowerLeft.y
        };

        for (int current_bullet = FIRST_IDX_BULLETS; current_bullet < ENTITIES_ARRAY_SIZE; ++current_bullet) {
            if (!entities[current_bullet].alive) continue;
            if (entities[current_bullet].shotSrc == entities[i].type) continue;
            if (
                (entities[i].type >= ENEMY_SHIP && entities[i].type <= ENEMY_FAST) &&
                (entities[current_bullet].shotSrc >= ENEMY_SHIP && entities[current_bullet].shotSrc <= ENEMY_FAST)
            ) continue;

            Vector2 bulletUpperLeft = entities[current_bullet].position;
            Vector2 bulletUpperRight = {
                bulletUpperLeft.x + BULLET_WIDTH,
                bulletUpperLeft.y
            };
            Vector2 bulletLowerLeft = {
                bulletUpperLeft.x,
                bulletUpperLeft.y + BULLET_HEIGHT
            };
            Vector2 bulletLowerRight = {
                bulletUpperLeft.x + BULLET_WIDTH,
                bulletLowerLeft.y
            };

            if (
                (bulletUpperRight.x >= upperLeft.x && bulletUpperRight.x <= upperRight.x && bulletUpperRight.y >= upperLeft.y && bulletUpperRight.y <= lowerLeft.y) ||
                (bulletUpperLeft.x >= upperLeft.x && bulletUpperLeft.x <= upperRight.x && bulletUpperLeft.y >= upperLeft.y && bulletUpperLeft.y <= lowerLeft.y) ||
                (bulletLowerRight.x >= lowerLeft.x && bulletLowerRight.x <= lowerRight.x && bulletLowerRight.y >= upperLeft.y && bulletLowerRight.y <= lowerLeft.y) ||
                (bulletLowerLeft.x >= lowerLeft.x && bulletLowerLeft.x <= lowerRight.x && bulletLowerLeft.y >= upperLeft.y && bulletLowerLeft.y <= lowerLeft.y)
            ) {
                entities[i].alive = false;
                entities[current_bullet].alive = false;

                if (i == gameData->firstAlive) {
                    for (int j = i+1; j < FIRST_IDX_BULLETS && !entities[gameData->firstAlive].alive; ++j) {
                        if (entities[j].alive) gameData->firstAlive = j;
                    }
                }

                if (entities[i].type != SHIP) {
                    gameData->score += entities[i].points;
                    PlaySound(gameData->alienExplosion);
                } else {
                    gameData->gameState = LOSE;
                    PlaySound(gameData->shipExplosion);
                } 
            }
        }
    }
}

void enemyAI(GameData *gameData) {
    int enemyIndex = rand() % (N_ENEMIES*30);
    if (enemyIndex > ENEMY_SHIP && enemyIndex < FIRST_IDX_BULLETS)
        fire(gameData, enemyIndex);
}

void updateGame(GameData *gameData) {
    UpdateMusicStream(gameData->BGMusic);
    if (gameData->gameState != PLAYING) return;

    detectCollisions(gameData);
    enemyAI(gameData);

    bool changeDirection = false;
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        Entity *entities = gameData->entities;
        if (!entities[i].alive) continue;
        if (i == SHIP) {
            float nextShipPositionX = entities[SHIP].position.x + entities[SHIP].velocity.x;
            // If the next update was going to put player ship beyond limit put it on the limit and put 0 on velocity.x
            if (nextShipPositionX + SHIP_WIDTH > SCREEN_LIMIT_RIGHT) {
                entities[SHIP].velocity.x = 0.0f;
                entities[SHIP].position.x = SCREEN_LIMIT_RIGHT - SHIP_WIDTH;
            } else if (nextShipPositionX < SCREEN_LIMIT_LEFT) {
                entities[SHIP].velocity.x = 0.0f;
                entities[SHIP].position.x = SCREEN_LIMIT_LEFT;
            }
        }
        // Check if the horde needs to change direction
        if (i == gameData->firstAlive && gameData->canCollideOnWall) {
            int enemyCol = (i-2)%ENEMIES_PER_ROW;
            int distLeft = entities[i].position.x - (ENEMIES_WIDTH+ENEMIES_GAP_X)*enemyCol;
            int distRight = entities[i].position.x + ENEMIES_WIDTH + (ENEMIES_WIDTH+ENEMIES_GAP_X)*(ENEMIES_PER_ROW-enemyCol-1);
            if (distLeft - HORDE_LIMIT_LEFT <= 0 || HORDE_LIMIT_RIGHT - distRight <= 0) {
                changeDirection = true;
                gameData->incrementPerLevel *= -1;
                gameData->canCollideOnWall = false;
                gameData->timeLastWallCollision = GetTime();
            }
        }

        if (changeDirection && i < FIRST_IDX_BULLETS) {
            entities[i].velocity.x *= -1;

            entities[i].velocity.x += gameData->incrementPerLevel;
            entities[i].velocity.y += 40.0f;
        }

        entities[i].position.x += entities[i].velocity.x;
        entities[i].position.y += entities[i].velocity.y;

        if (entities[i].type == SHIP) {
            entities[i].velocity.x = 0.0f;
        }

        if (entities[i].type != BULLET) {
            entities[i].velocity.y = 0.0f;
            double currentTime = GetTime();
            if (currentTime - entities[i].lastShotTime > entities[i].delayToFire) {
                entities[i].canFire = true;
            }
        } else if (entities[i].position.y < 0.0f || entities[i].position.y > 1080.0f) {
            entities[i].alive = false;
        }
        
        if (GetTime()-gameData->timeLastWallCollision > gameData->wallCollisionDelay) {
            gameData->canCollideOnWall = true;
        }
    }
}

void processInput(GameData *gameData) {
    switch (gameData->gameState) {
        // case MENU:
        // {

        // } break;
        case PLAYING:
        {
            if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
                gameData->gameState = PAUSED;
            }
            if (IsKeyDown(KEY_LEFT)) gameData->entities[0].velocity.x = -10;
            if (IsKeyDown(KEY_RIGHT)) gameData->entities[0].velocity.x = 10;
            if (IsKeyDown(KEY_SPACE)) fire(gameData, SHIP);
        } break;
        case PAUSED:
        {
            if (IsKeyPressed(KEY_Y)) gameData->exitWindow = true;
            else if (IsKeyPressed(KEY_N)) gameData->gameState = PLAYING;
        } break;
        // case WIN:
        // {

        // } break;
        // case LOSE:
        // {

        // } break;
        // While WIN and LOSE are not implmemented
        default:
        {
            if (IsKeyPressed(KEY_Y)) gameData->exitWindow = true;
            else if (IsKeyPressed(KEY_N)) gameData->gameState = PLAYING;
        } break;
    }
}

int main(void) {
    GameData gameData = initGame();

    while (!gameData.exitWindow) {
        processInput(&gameData);
        updateGame(&gameData);

        // Without 'BeginDrawing()' and 'EndDrawing()' the mainloop doesn't work
        BeginDrawing();
            drawGame(&gameData);
        EndDrawing();
    }

    closeGame(&gameData);

    return 0;
}