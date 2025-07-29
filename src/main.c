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
# define ENEMIES_OFFSET_Y (ENEMIES_WIDTH*3)
# define N_ENEMIES_ROWS 6
# define ENEMIES_PER_ROW 12
# define N_ENEMIES (N_ENEMIES_ROWS*ENEMIES_PER_ROW)
# define EXIT_BANNER_HEIGHT 200
# define EXIT_MESSAGE_FONT_SIZE 75
# define BULLET_SPEED 75 // pixels/frame


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
    Color color;
    bool alive;
    float delayToShoot;
} Entity;

Entity *buildEntities(Color colors[], float delayToShoot[]) {
    EntityType entityType = SHIP;

    Entity *entities = (Entity *)malloc((N_ENEMIES + 2) * sizeof(Entity));
    // Setup player ship
    entities[0].type = entityType;
    entities[0].dimensions.x = SHIP_WIDTH;
    entities[0].dimensions.y = SHIP_HEIGHT;
    entities[0].position.x = (SCREEN_WIDTH - SHIP_WIDTH)/2;
    entities[0].position.y = SHIP_POS_Y;
    entities[0].color = colors[0];
    entities[0].alive = true;
    entities[0].delayToShoot = delayToShoot[entityType];

    int enemiesXOffSet = (SCREEN_WIDTH - ENEMIES_PER_ROW*(ENEMIES_WIDTH + ENEMIES_GAP_X))/2;
    for (int i = 0; i < N_ENEMIES; ++i) {
        if (i % (N_ENEMIES/2) == 0) entityType++;
        entities[i+2].type = entityType;
        entities[i+2].dimensions.x = ENEMIES_WIDTH;
        entities[i+2].dimensions.y = ENEMIES_WIDTH;
        entities[i+2].position.x = enemiesXOffSet + (ENEMIES_WIDTH + ENEMIES_GAP_X)*(i%ENEMIES_PER_ROW);
        entities[i+2].position.y = ENEMIES_OFFSET_Y + (ENEMIES_WIDTH + ENEMIES_GAP_Y)*(i/ENEMIES_PER_ROW);
        entities[i+2].color = colors[entityType];
        entities[i+2].alive = true;
        entities[i+2].delayToShoot = delayToShoot[entityType];
    }

    // Setup Enemy ship
    entities[1].type = ++entityType;
    entities[1].dimensions.x = ENEMY_SHIP_WIDTH;
    entities[1].dimensions.y = ENEMY_SHIP_HEIGHT;
    entities[1].position.x = (SCREEN_WIDTH - ENEMY_SHIP_WIDTH)/2;
    entities[1].position.y = ENEMY_SHIP_POS_Y;
    entities[1].color = colors[entityType];
    entities[1].alive = false;
    entities[1].delayToShoot = delayToShoot[entityType];

    return entities;
}

void drawEntites(Entity *entities) {
    for (int i = 0; i < N_ENEMIES+2; ++i) {
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
    for (int i = 0; i < N_ENEMIES+2; ++i) {
        printf("Entity at index: %d\n", i);
        printf("\tType: %d\n", entities[i].type);
        printf("\tWidth: %lf\n", entities[i].dimensions.x);
        printf("\tHeight: %lf\n", entities[i].dimensions.y);
        printf("\tposX: %lf\n", entities[i].position.x);
        printf("\tposY: %lf\n", entities[i].position.y);
        printf("\tDelay to shoot: %.2lf\n", entities[i].delayToShoot);
    }
}

int main(void) {
    srand(time(NULL));
    Color colors[] = {RAYWHITE, DARKPURPLE, DARKGREEN, YELLOW};
    float delayToShoot[] = {0.5f, 3.0f, 1.0f, 0.1f};

    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Space Invaders clone using raylib"
    );

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    DisableCursor();
    Entity *entities = buildEntities(colors, delayToShoot);
    bool exitWindowRequested = false;
    bool exitWindow = false;
    Font defaultFont = GetFontDefault();

    while (!exitWindow) {
        if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE))
            exitWindowRequested = true;
        if (exitWindowRequested) {
            if (IsKeyPressed(KEY_Y)) exitWindow = true;
            else if (IsKeyPressed(KEY_N)) exitWindowRequested = false;
        }

        if (IsKeyDown(KEY_LEFT)) entities[0].position.x -= 10;
        if (IsKeyDown(KEY_RIGHT)) entities[0].position.x += 10;

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