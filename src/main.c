# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include "raylib.h"


# define SCREEN_WIDTH 1920
# define SCREEN_HEIGHT 1080
# define SHIP_WIDTH 150
# define SHIP_HEIGHT 50
# define SHIP_POS_Y 900
# define ENEMY_SHIP_WIDTH SHIP_WIDTH
# define ENEMY_SHIP_HEIGHT SHIP_HEIGHT
# define ENEMY_SHIP_POS_Y 50
# define ENEMIES_GAP_X 10
# define ENEMIES_GAP_Y 15
# define ENEMIES_WIDTH 40
# define ENEMIES_HEIGHT ENEMIES_WIDTH
# define ENEMIES_OFFSET_Y (ENEMIES_WIDTH*3)
# define N_ENEMIES_ROWS 6
# define ENEMIES_PER_ROW 12
# define N_ENEMIES (N_ENEMIES_ROWS*ENEMIES_PER_ROW)
# define EXIT_BANNER_HEIGHT 200
# define EXIT_MESSAGE_FONT_SIZE 75
# define BULLET_SPEED 20 // pixels/frame
# define N_BULLETS 100
# define BULLET_WIDTH 20
# define BULLET_HEIGHT 40
# define ENTITIES_ARRAY_SIZE (2+N_ENEMIES+N_BULLETS)


const char exitMessage[] = "Are you sure you want to exit game? [Y/N]";

typedef enum EntityType {
    SHIP,
    ENEMY_SLOW,
    ENEMY_FAST,
    ENEMY_SHIP,
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
} Entity;

Entity *buildEntities(Color colors[], float delayToFire[]) {
    EntityType entityType = SHIP;

    Entity *entities = (Entity *)malloc((ENTITIES_ARRAY_SIZE) * sizeof(Entity));
    // Setup player ship
    entities[0].type = entityType;
    entities[0].dimensions.x = SHIP_WIDTH;
    entities[0].dimensions.y = SHIP_HEIGHT;
    entities[0].position.x = (SCREEN_WIDTH - SHIP_WIDTH)/2;
    entities[0].position.y = SHIP_POS_Y;
    entities[0].velocity.x = 0.0f;
    entities[0].velocity.y = 0.0f;
    entities[0].color = colors[0];
    entities[0].alive = true;
    entities[0].delayToFire = delayToFire[entityType];
    entities[0].canShot = true;
    entities[0].lastShotTime = GetTime();

    int enemiesXOffSet = (SCREEN_WIDTH - ENEMIES_PER_ROW*(ENEMIES_WIDTH + ENEMIES_GAP_X))/2;
    for (int i = 0; i < N_ENEMIES; ++i) {
        if (i % (N_ENEMIES/2) == 0) entityType++;
        entities[i+2].type = entityType;
        entities[i+2].dimensions.x = ENEMIES_WIDTH;
        entities[i+2].dimensions.y = ENEMIES_WIDTH;
        entities[i+2].position.x = enemiesXOffSet + (ENEMIES_WIDTH + ENEMIES_GAP_X)*(i%ENEMIES_PER_ROW);
        entities[i+2].position.y = ENEMIES_OFFSET_Y + (ENEMIES_WIDTH + ENEMIES_GAP_Y)*(i/ENEMIES_PER_ROW);
        entities[i+2].velocity.x = 0.0f;
        entities[i+2].velocity.y = 0.0f;
        entities[i+2].color = colors[entityType];
        entities[i+2].alive = true;
        entities[i+2].delayToFire = delayToFire[entityType];
        entities[i+2].canShot = true;
        entities[i+2].lastShotTime = GetTime();
    }

    // Setup Enemy ship
    entities[1].type = ++entityType;
    entities[1].dimensions.x = ENEMY_SHIP_WIDTH;
    entities[1].dimensions.y = ENEMY_SHIP_HEIGHT;
    entities[1].position.x = (SCREEN_WIDTH - ENEMY_SHIP_WIDTH)/2;
    entities[1].position.y = ENEMY_SHIP_POS_Y;
    entities[1].velocity.x = 5.0f;
    entities[1].velocity.y = 0.0f;
    entities[1].color = colors[entityType];
    entities[1].alive = false;
    entities[1].delayToFire = delayToFire[entityType++];
    entities[1].canShot = true;
    entities[1].lastShotTime = GetTime();

    for (int i = N_ENEMIES + 2; i < ENTITIES_ARRAY_SIZE; ++i) {
        entities[i].type = entityType;
        entities[i].dimensions.x = BULLET_WIDTH;
        entities[i].dimensions.y = BULLET_HEIGHT;
        entities[i].alive = false;
        entities[i].color = RAYWHITE;
    }

    return entities;
}

void drawEntites(Entity *entities) {
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

void updateEntities(Entity *entities) {
    for (int i = 0; i < ENTITIES_ARRAY_SIZE; ++i) {
        if (entities[i].alive) {
            entities[i].position.x += entities[i].velocity.x;
            entities[i].position.y += entities[i].velocity.y;
            if (entities[i].type == SHIP) {
                entities[i].velocity.x = 0.0f;
                entities[i].velocity.y = 0.0f;
            }
            if (entities[i].type != BULLET) {
                double currentTime = GetTime();
                if (currentTime - entities[i].lastShotTime > entities[i].delayToFire) {
                    entities[i].canShot = true;
                }
            }
            if (entities[i].position.y < 0.0f) entities[i].alive = false;
        }
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

int main(void) {
    srand(time(NULL));
    Color colors[] = {RAYWHITE, DARKPURPLE, DARKGREEN, YELLOW};
    float delayToFire[] = {0.5f, 3.0f, 1.0f, 0.1f};

    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Space Invaders clone using raylib"
    );

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    DisableCursor();
    Entity *entities = buildEntities(colors, delayToFire);
    bool exitWindowRequested = false;
    bool exitWindow = false;

    while (!exitWindow) {
        if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE))
            exitWindowRequested = true;
        if (exitWindowRequested) {
            if (IsKeyPressed(KEY_Y)) exitWindow = true;
            else if (IsKeyPressed(KEY_N)) exitWindowRequested = false;
        } else {
            // Input processing
            if (IsKeyDown(KEY_LEFT)) entities[0].velocity.x = -10;
            if (IsKeyDown(KEY_RIGHT)) entities[0].velocity.x = 10;
            if (IsKeyDown(KEY_SPACE) && entities[0].canShot) shot(entities, 0);
    
            updateEntities(entities);
        }

        // Without 'BeginDrawing()' and 'EndDrawing()' the mainloop doesn't work
        BeginDrawing();
            ClearBackground(BLACK);
            drawEntites(entities);

            if (exitWindowRequested) drawExitMessage();
        EndDrawing();
    }

    CloseWindow();
    free(entities);

    return 0;
}