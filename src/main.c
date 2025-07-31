# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include "raylib.h"


# define SCREEN_WIDTH 1920
# define SCREEN_HEIGHT 1080
# define SHIP_WIDTH 96
# define SHIP_HEIGHT 72
# define SHIP_POS_Y 900
# define ENEMY_SHIP_WIDTH SHIP_WIDTH
# define ENEMY_SHIP_HEIGHT SHIP_HEIGHT
# define ENEMY_SHIP_POS_Y 50
# define ENEMIES_GAP_X 25
# define ENEMIES_GAP_Y 15
# define ENEMIES_WIDTH 32
# define ENEMIES_HEIGHT ENEMIES_WIDTH
# define ENEMIES_HEIGHT ENEMIES_WIDTH
# define ENEMIES_OFFSET_Y (ENEMIES_WIDTH*3)
# define N_ENEMIES_ROWS 4
# define ENEMIES_PER_ROW 11
# define N_ENEMIES (N_ENEMIES_ROWS*ENEMIES_PER_ROW)
# define EXIT_BANNER_HEIGHT 200
# define EXIT_MESSAGE_FONT_SIZE 75
# define BULLET_SPEED 20 // pixels/frame
# define N_BULLETS 10000
# define BULLET_WIDTH 4
# define BULLET_HEIGHT 32
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

typedef struct AnimationFrame {
    int nFrames;
    int currentFrame;
    Rectangle frameBounds;
} AnimationFrame;

typedef struct Entity {
    EntityType type;
    Rectangle bounds;
    AnimationFrame animationFrame;
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
    Entity *entities;
    GameState gameState;
    Music BGMusic;
    Sound shipFire;
    Sound alienFire;
    Sound shipExplosion;
    Sound alienExplosion;
    Texture2D shipSingleShot;
    Texture2D singleBullet;
    Texture2D enemyFast;
    Texture2D enemySlow;
} GameData;

Entity *buildEntities(const Color colors[], const float delayToFire[]) {
    Entity *entities = (Entity *)malloc((ENTITIES_ARRAY_SIZE) * sizeof(Entity));
    // Setup player ship
    entities[SHIP].type = SHIP;
    entities[SHIP].bounds.x = (SCREEN_WIDTH - SHIP_WIDTH)/2;
    entities[SHIP].bounds.y = SHIP_POS_Y;
    entities[SHIP].bounds.width = SHIP_WIDTH;
    entities[SHIP].bounds.height = SHIP_HEIGHT;
    entities[SHIP].animationFrame.nFrames = 2;
    entities[SHIP].animationFrame.currentFrame = 1;
    entities[SHIP].animationFrame.frameBounds.x = 16.0f;
    entities[SHIP].animationFrame.frameBounds.y = 0.0f;
    entities[SHIP].animationFrame.frameBounds.width = 16.0f;
    entities[SHIP].animationFrame.frameBounds.height = 12.0f;
    entities[SHIP].velocity.x = 0.0f;
    entities[SHIP].velocity.y = 0.0f;
    entities[SHIP].color = colors[0];
    entities[SHIP].alive = true;
    entities[SHIP].delayToFire = delayToFire[SHIP];
    entities[SHIP].canFire = true;
    entities[SHIP].lastShotTime = GetTime();
    entities[SHIP].points = 0;

    int enemiesXOffSet = (SCREEN_WIDTH - ENEMIES_PER_ROW*(ENEMIES_WIDTH + ENEMIES_GAP_X))/2;
    EntityType entityType = ENEMY_SHIP;
    for (int i = 0; i < N_ENEMIES; ++i) {
        if (i % (N_ENEMIES/2) == 0) entityType++;
        entities[i+2].type = entityType;
        entities[i+2].bounds.x = enemiesXOffSet + (ENEMIES_WIDTH + ENEMIES_GAP_X)*(i%ENEMIES_PER_ROW);
        entities[i+2].bounds.y = ENEMIES_OFFSET_Y + (ENEMIES_WIDTH + ENEMIES_GAP_Y)*(i/ENEMIES_PER_ROW);
        entities[i+2].bounds.width = ENEMIES_WIDTH;
        entities[i+2].bounds.height = ENEMIES_HEIGHT;
        entities[i+2].animationFrame.nFrames = 4;
        entities[i+2].animationFrame.currentFrame = rand() % 4;
        entities[i+2].animationFrame.frameBounds.x = 0.0f;
        entities[i+2].animationFrame.frameBounds.y = 0.0f;
        entities[i+2].animationFrame.frameBounds.width = 16.0f;
        entities[i+2].animationFrame.frameBounds.height = 16.0f;
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
    entities[ENEMY_SHIP].bounds.x = (SCREEN_WIDTH - ENEMY_SHIP_WIDTH)/2;
    entities[ENEMY_SHIP].bounds.y = ENEMY_SHIP_POS_Y;
    entities[ENEMY_SHIP].bounds.width = ENEMY_SHIP_WIDTH;
    entities[ENEMY_SHIP].bounds.height = ENEMY_SHIP_HEIGHT;
    entities[ENEMY_SHIP].animationFrame.nFrames = 2;
    entities[ENEMY_SHIP].animationFrame.currentFrame = 0;
    entities[ENEMY_SHIP].animationFrame.frameBounds.x = 0.0f;
    entities[ENEMY_SHIP].animationFrame.frameBounds.y = 0.0f;
    entities[ENEMY_SHIP].animationFrame.frameBounds.width = 16.0f;
    entities[ENEMY_SHIP].animationFrame.frameBounds.height = 16.0f;
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
        entities[i].bounds.width = BULLET_WIDTH;
        entities[i].bounds.height = BULLET_HEIGHT;
        entities[i].animationFrame.nFrames = 1;
        entities[i].animationFrame.currentFrame = 0;
        entities[i].animationFrame.frameBounds.x = 0.0f;
        entities[i].animationFrame.frameBounds.y = 0.0f;
        entities[i].animationFrame.frameBounds.width = 2.0f;
        entities[i].animationFrame.frameBounds.height = 8.0f;
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
    SetTargetFPS(30);
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
        .BGMusic=LoadMusicStream("resources/BGMusic.ogg"),
        .shipFire=LoadSound("resources/shipFire.ogg"),
        .alienFire=LoadSound("resources/alienFire.ogg"),
        .shipExplosion=LoadSound("resources/shipExplosion.ogg"),
        .alienExplosion=LoadSound("resources/alienExplosion.ogg"),
        .shipSingleShot=LoadTexture("resources/shipSingleShot.png"),
        .enemySlow=LoadTexture("resources/enemySlow.png"),
        .enemyFast=LoadTexture("resources/enemyFast.png"),
        .singleBullet=LoadTexture("resources/singleBullet.png"),
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
    UnloadTexture(gameData->shipSingleShot);
    UnloadTexture(gameData->enemyFast);
    UnloadTexture(gameData->enemySlow);
    UnloadTexture(gameData->singleBullet);
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

void drawEntities(GameData *gameData) {
    const Vector2 origin = {0.0f, 0.0f};
    Entity *entities = gameData->entities;
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        if (entities[i].alive) {
            Texture2D currentTexture;
            switch (entities[i].type) {
                case SHIP:
                {
                    currentTexture = gameData->shipSingleShot;
                } break;
                case ENEMY_SLOW:
                {
                    currentTexture = gameData->enemySlow;
                } break;
                case ENEMY_FAST:
                {
                    currentTexture = gameData->enemyFast;
                } break;
                case BULLET:
                {
                    currentTexture = gameData->singleBullet;
                } break;
                default: break;
            }

            /*
                When `origin` is not 0 the sprite is inserted with an offset,
                that's why the bug on collision detection
            */
            DrawTexturePro(
                currentTexture,
                entities[i].animationFrame.frameBounds,
                entities[i].bounds,
                origin,
                0.0f,
                WHITE
            );
        }
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
            drawEntities(gameData);
        } break;
        case PAUSED:
        {
            drawEntities(gameData);
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
            drawEntities(gameData);
            drawExitMessage();
        } break;
    }
}

void debugEntities(Entity *entities) {
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        printf("Entity at index: %d\n", i);
        printf("\tType: %d\n", entities[i].type);
        printf("\tWidth: %lf\n", entities[i].bounds.width);
        printf("\tHeight: %lf\n", entities[i].bounds.height);
        printf("\tposX: %lf\n", entities[i].bounds.x);
        printf("\tposY: %lf\n", entities[i].bounds.y);
        printf("\tDelay to shoot: %.2lf\n", entities[i].delayToFire);
    }
}

void fire(GameData *gameData, int shooterIdx) {
    if (!gameData->entities[shooterIdx].canFire) return;
    if (!gameData->entities[shooterIdx].alive) return;

    Entity *entities = gameData->entities;
    // There are a small number of bullets, so for now a linear search is ok
    int firstBulletAvailable;
    for (firstBulletAvailable = FIRST_IDX_BULLETS; firstBulletAvailable < ENTITIES_ARRAY_SIZE && entities[firstBulletAvailable].alive; ++firstBulletAvailable);
    if (!entities[firstBulletAvailable].alive) {
        entities[shooterIdx].canFire = false;
        entities[shooterIdx].lastShotTime = GetTime();

        entities[firstBulletAvailable].bounds.x = entities[shooterIdx].bounds.x + 0.5 * (entities[shooterIdx].bounds.width-BULLET_WIDTH);
        entities[firstBulletAvailable].alive = true;
        entities[firstBulletAvailable].shotSrc = entities[shooterIdx].type;

        if (entities[shooterIdx].type == SHIP) {
            entities[firstBulletAvailable].velocity.y = -BULLET_SPEED;
            entities[firstBulletAvailable].bounds.y = entities[shooterIdx].bounds.y - BULLET_HEIGHT;
            PlaySound(gameData->shipFire);
        } else {
            entities[firstBulletAvailable].velocity.y = BULLET_SPEED;
            entities[firstBulletAvailable].bounds.y = entities[shooterIdx].bounds.y + entities[shooterIdx].bounds.height;
            PlaySound(gameData->alienFire);
        }
    }
}


// Implements a naive collision detection
void detectCollisions(GameData *gameData) {
    Entity *entities = gameData->entities;
    for (int i = 0; i < FIRST_IDX_BULLETS; ++i) {
        if (!entities[i].alive) continue;

        Vector2 upperLeft = {entities[i].bounds.x, entities[i].bounds.y};
        Vector2 upperRight = {
            upperLeft.x + entities[i].bounds.width,
            upperLeft.y
        };
        Vector2 lowerLeft = {
            upperLeft.x,
            upperLeft.y + entities[i].bounds.height
        };
        Vector2 lowerRight = {
            lowerLeft.x + entities[i].bounds.width,
            lowerLeft.y
        };

        for (int current_bullet = FIRST_IDX_BULLETS; current_bullet < ENTITIES_ARRAY_SIZE; ++current_bullet) {
            if (!entities[current_bullet].alive) continue;
            if (entities[current_bullet].shotSrc == entities[i].type) continue;
            if (
                (entities[i].type >= ENEMY_SHIP && entities[i].type <= ENEMY_FAST) &&
                (entities[current_bullet].shotSrc >= ENEMY_SHIP && entities[current_bullet].shotSrc <= ENEMY_FAST)
            ) continue;

            Vector2 bulletUpperLeft = {entities[current_bullet].bounds.x, entities[current_bullet].bounds.y};
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
                    entities[i].points += entities[i].points;
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
    int enemyIndex = rand() % (N_ENEMIES*20);
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
            float nextShipPositionX = entities[SHIP].bounds.x + entities[SHIP].velocity.x;
            // If the next update was going to put player ship beyond limit put it on the limit and put 0 on velocity.x
            if (nextShipPositionX + SHIP_WIDTH > SCREEN_LIMIT_RIGHT) {
                entities[SHIP].velocity.x = 0.0f;
                entities[SHIP].bounds.x = SCREEN_LIMIT_RIGHT - SHIP_WIDTH;
            } else if (nextShipPositionX < SCREEN_LIMIT_LEFT) {
                entities[SHIP].velocity.x = 0.0f;
                entities[SHIP].bounds.x = SCREEN_LIMIT_LEFT;
            }
        } else if (entities[i].type != BULLET) {
            entities[i].animationFrame.currentFrame = (entities[i].animationFrame.currentFrame+1) % entities[i].animationFrame.nFrames;
            entities[i].animationFrame.frameBounds.x = entities[i].animationFrame.frameBounds.width*entities[i].animationFrame.currentFrame;
        }
        // Check if the horde needs to change direction
        if (i == gameData->firstAlive && gameData->canCollideOnWall) {
            int enemyCol = (i-2)%ENEMIES_PER_ROW;
            int distLeft = entities[i].bounds.x - (ENEMIES_WIDTH+ENEMIES_GAP_X)*enemyCol;
            int distRight = entities[i].bounds.x + ENEMIES_WIDTH + (ENEMIES_WIDTH+ENEMIES_GAP_X)*(ENEMIES_PER_ROW-enemyCol-1);
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

        entities[i].bounds.x += entities[i].velocity.x;
        entities[i].bounds.y += entities[i].velocity.y;

        if (entities[i].type == SHIP) {
            entities[i].velocity.x = 0.0f;
        }

        if (entities[i].type != BULLET) {
            entities[i].velocity.y = 0.0f;
            double currentTime = GetTime();
            if (currentTime - entities[i].lastShotTime > entities[i].delayToFire) {
                entities[i].canFire = true;
            }
        } else if (entities[i].bounds.y < 0.0f || entities[i].bounds.y > 1080.0f) {
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