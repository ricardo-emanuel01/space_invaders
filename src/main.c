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
# define N_BULLETS 100
# define BULLET_WIDTH 5
# define BULLET_HEIGHT 40
# define ENTITIES_ARRAY_SIZE (2+N_ENEMIES+N_BULLETS)
# define FIRST_IDX_BULLETS (2+N_ENEMIES)
# define SCREEN_LIMIT 400
# define LIMIT_LEFT SCREEN_LIMIT
# define LIMIT_RIGHT (SCREEN_WIDTH-SCREEN_LIMIT)
# define WALL_COLISION_DELAY 1.0f


const char exitMessage[] = "Are you sure you want to exit game? [Y/N]";

typedef enum EntityType {
    SHIP,
    ENEMY_SHIP,
    ENEMY_SLOW,
    ENEMY_FAST,
    BULLET,
} EntityType;

typedef struct Entity {
    EntityType type;
    Vector2 dimensions;
    Vector2 position;
    Vector2 velocity;
    Color color;
    bool alive;
    float delayToFire;
    bool canShot;
    double lastShotTime;
    EntityType shotSrc;
} Entity;

typedef struct GameState {
    bool exitWindowRequested;
    bool exitWindow;
    bool canCollideOnWall;
    double timeLastWallCollision;
    float wallCollisionDelay;
    int firstAlive;
    Entity *entities;
} GameState;

Entity *buildEntities(const Color colors[], const float delayToFire[]) {
    EntityType entityType = SHIP;

    Entity *entities = (Entity *)malloc((ENTITIES_ARRAY_SIZE) * sizeof(Entity));
    // Setup player ship
    entities[SHIP].type = entityType;
    entities[SHIP].dimensions.x = SHIP_WIDTH;
    entities[SHIP].dimensions.y = SHIP_HEIGHT;
    entities[SHIP].position.x = (SCREEN_WIDTH - SHIP_WIDTH)/2;
    entities[SHIP].position.y = SHIP_POS_Y;
    entities[SHIP].velocity.x = 0.0f;
    entities[SHIP].velocity.y = 0.0f;
    entities[SHIP].color = colors[0];
    entities[SHIP].alive = true;
    entities[SHIP].delayToFire = delayToFire[entityType];
    entities[SHIP].canShot = true;
    entities[SHIP].lastShotTime = GetTime();

    int enemiesXOffSet = (SCREEN_WIDTH - ENEMIES_PER_ROW*(ENEMIES_WIDTH + ENEMIES_GAP_X))/2;
    for (int i = 0; i < N_ENEMIES; ++i) {
        if (i % (N_ENEMIES/2) == 0) entityType++;
        entities[i+2].type = entityType;
        entities[i+2].dimensions.x = ENEMIES_WIDTH;
        entities[i+2].dimensions.y = ENEMIES_WIDTH;
        entities[i+2].position.x = enemiesXOffSet + (ENEMIES_WIDTH + ENEMIES_GAP_X)*(i%ENEMIES_PER_ROW);
        entities[i+2].position.y = ENEMIES_OFFSET_Y + (ENEMIES_WIDTH + ENEMIES_GAP_Y)*(i/ENEMIES_PER_ROW);
        entities[i+2].velocity.x = 2.0f;
        entities[i+2].velocity.y = 0.0f;
        entities[i+2].color = colors[entityType];
        entities[i+2].alive = true;
        entities[i+2].delayToFire = delayToFire[entityType];
        entities[i+2].canShot = true;
        entities[i+2].lastShotTime = GetTime();
    }

    // Setup Enemy ship
    entities[ENEMY_SHIP].type = ++entityType;
    entities[ENEMY_SHIP].dimensions.x = ENEMY_SHIP_WIDTH;
    entities[ENEMY_SHIP].dimensions.y = ENEMY_SHIP_HEIGHT;
    entities[ENEMY_SHIP].position.x = (SCREEN_WIDTH - ENEMY_SHIP_WIDTH)/2;
    entities[ENEMY_SHIP].position.y = ENEMY_SHIP_POS_Y;
    entities[ENEMY_SHIP].velocity.x = 5.0f;
    entities[ENEMY_SHIP].velocity.y = 0.0f;
    entities[ENEMY_SHIP].color = colors[entityType];
    entities[ENEMY_SHIP].alive = false;
    entities[ENEMY_SHIP].delayToFire = delayToFire[entityType++];
    entities[ENEMY_SHIP].canShot = true;
    entities[ENEMY_SHIP].lastShotTime = GetTime();

    for (int i = N_ENEMIES + 2; i < ENTITIES_ARRAY_SIZE; ++i) {
        entities[i].type = entityType;
        entities[i].dimensions.x = BULLET_WIDTH;
        entities[i].dimensions.y = BULLET_HEIGHT;
        entities[i].alive = false;
        entities[i].color = RAYWHITE;
    }

    return entities;
}

GameState initGame() {
    srand(time(NULL));
    const Color colors[] = {RAYWHITE, DARKPURPLE, DARKGREEN, YELLOW};
    const float delayToFire[] = {0.5f, 3.0f, 1.0f, 0.1f};
    GameState gameState = {
        .exitWindowRequested=false,
        .exitWindow=false,
        .canCollideOnWall=true,
        .timeLastWallCollision=0.0,
        .wallCollisionDelay=WALL_COLISION_DELAY,
        .firstAlive=2,
        .entities=buildEntities(colors, delayToFire)
    };

    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Space Invaders clone using raylib"
    );

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    DisableCursor();

    return gameState;
}

void drawEntites(Entity *entities) {
    ClearBackground(BLACK);
    DrawFPS(10, 10);
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        if (entities[i].alive)
            DrawRectangle(
                entities[i].position.x,
                entities[i].position.y,
                entities[i].dimensions.x,
                entities[i].dimensions.y,
                entities[i].color
            );
    }
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

void debugEntities(Entity *entities) {
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        printf("Entity at index: %d\n", i);
        printf("\tType: %d\n", entities[i].type);
        printf("\tWidth: %lf\n", entities[i].dimensions.x);
        printf("\tHeight: %lf\n", entities[i].dimensions.y);
        printf("\tposX: %lf\n", entities[i].position.x);
        printf("\tposY: %lf\n", entities[i].position.y);
        printf("\tDelay to shoot: %.2lf\n", entities[i].delayToFire);
    }
}

void shot(Entity *entities, int shooterIdx) {
    // There are a small number of bullets, so for now a linear search is ok
    for (int i = N_ENEMIES + 2; i < ENTITIES_ARRAY_SIZE; ++i) {
        if (!entities[i].alive) {
            entities[shooterIdx].canShot = false;
            entities[shooterIdx].lastShotTime = GetTime();

            entities[i].position.x = entities[shooterIdx].position.x + 0.5*entities[shooterIdx].dimensions.x;
            entities[i].alive = true;
            entities[i].shotSrc = entities[shooterIdx].type;

            if (entities[shooterIdx].type == SHIP) {
                entities[i].velocity.y = -BULLET_SPEED;
                entities[i].position.y = entities[shooterIdx].position.y;
            } else {
                entities[i].velocity.y = BULLET_SPEED;
                entities[i].position.y = entities[shooterIdx].position.y + entities[shooterIdx].dimensions.y;
            }
            break;
        }
    }
}

// Implements a naive collision detection
void detectCollision(GameState *gameState) {
    for (int i = 0; i < FIRST_IDX_BULLETS; ++i) {
        if (!gameState->entities[i].alive) continue;

        Vector2 upperLeft = gameState->entities[i].position;
        Vector2 upperRight = {
            upperLeft.x + gameState->entities[i].dimensions.x,
            upperLeft.y
        };
        Vector2 lowerLeft = {
            upperLeft.x,
            upperLeft.y + gameState->entities[i].dimensions.y
        };
        Vector2 lowerRight = {
            lowerLeft.x + gameState->entities[i].dimensions.x,
            lowerLeft.y
        };

        for (int current_bullet = FIRST_IDX_BULLETS; current_bullet < ENTITIES_ARRAY_SIZE; ++current_bullet) {
            if (!gameState->entities[current_bullet].alive) continue;
            if (gameState->entities[current_bullet].shotSrc == gameState->entities[i].type) continue;
            if (
                (gameState->entities[i].type >= ENEMY_SHIP && gameState->entities[i].type <= ENEMY_FAST) &&
                (gameState->entities[current_bullet].shotSrc >= ENEMY_SHIP && gameState->entities[current_bullet].shotSrc <= ENEMY_FAST)
            ) continue;

            Vector2 bulletUpperLeft = gameState->entities[current_bullet].position;
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
                gameState->entities[i].alive = false;
                gameState->entities[current_bullet].alive = false;

                if (i == gameState->firstAlive) {
                    for (int j = i+1; j < FIRST_IDX_BULLETS && !gameState->entities[gameState->firstAlive].alive; ++j) {
                        if (gameState->entities[j].alive) gameState->firstAlive = j;
                    }
                }
            }
        }
    }
}

void updateEntities(GameState *gameState) {
    bool changeDirection = false;
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        if (!gameState->entities[i].alive) continue;
        // Check if the hord needs to change direction
        if (i == gameState->firstAlive && gameState->canCollideOnWall) {
            int enemyCol = (i-2)%ENEMIES_PER_ROW;
            int distLeft = gameState->entities[i].position.x - (ENEMIES_WIDTH+ENEMIES_GAP_X)*enemyCol;
            int distRight = gameState->entities[i].position.x + ENEMIES_WIDTH + (ENEMIES_WIDTH+ENEMIES_GAP_X)*(ENEMIES_PER_ROW-enemyCol-1);
            if (distLeft - LIMIT_LEFT <= 0 || LIMIT_RIGHT - distRight <= 0) {
                changeDirection = true;
                gameState->canCollideOnWall = false;
                gameState->timeLastWallCollision = GetTime();
            }
        }

        if (changeDirection && i < FIRST_IDX_BULLETS) {
            gameState->entities[i].velocity.x *= -1;
            gameState->entities[i].velocity.y += 30.0f;
        }

        gameState->entities[i].position.x += gameState->entities[i].velocity.x;
        gameState->entities[i].position.y += gameState->entities[i].velocity.y;

        if (gameState->entities[i].type == SHIP) {
            gameState->entities[i].velocity.x = 0.0f;
        }

        if (gameState->entities[i].type != BULLET) {
            gameState->entities[i].velocity.y = 0.0f;
            double currentTime = GetTime();
            if (currentTime - gameState->entities[i].lastShotTime > gameState->entities[i].delayToFire) {
                gameState->entities[i].canShot = true;
            }
        } else if (gameState->entities[i].position.y < 0.0f || gameState->entities[i].position.y > 1080.0f) {
            gameState->entities[i].alive = false;
        }
        
        if (GetTime()-gameState->timeLastWallCollision > gameState->wallCollisionDelay) {
            gameState->canCollideOnWall = true;
        }
    }
}

void processInput(GameState *gameState) {
    if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
        gameState->exitWindowRequested = true;
    }

    if (gameState->exitWindowRequested) {
        if (IsKeyPressed(KEY_Y)) gameState->exitWindow = true;
        else if (IsKeyPressed(KEY_N)) gameState->exitWindowRequested = false;
    }

    if (IsKeyDown(KEY_LEFT)) gameState->entities[0].velocity.x = -10;
    if (IsKeyDown(KEY_RIGHT)) gameState->entities[0].velocity.x = 10;
    if (IsKeyDown(KEY_SPACE) && gameState->entities[0].canShot) shot(gameState->entities, 0);
}

int main(void) {
    GameState gameState = initGame();

    while (!gameState.exitWindow) {
        processInput(&gameState);
        if (!gameState.exitWindowRequested) {
            detectCollision(&gameState);
            updateEntities(&gameState);
        }

        // Without 'BeginDrawing()' and 'EndDrawing()' the mainloop doesn't work
        BeginDrawing();
            drawEntites(gameState.entities);
            if (gameState.exitWindowRequested) drawExitMessage();
        EndDrawing();
    }

    CloseWindow();
    free(gameState.entities);

    return 0;
}