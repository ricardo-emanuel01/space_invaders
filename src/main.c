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


const char exitMessage[] = "Are you sure you want to exit game? [Y/N]";

typedef enum EntityType {
    SHIP,
    BULLET,
    ENEMY_SLOW,
    ENEMY_FAST,
    ENEMY_SHIP,
} EntityType;

typedef struct Entity {
    EntityType type;
    int width;
    int height;
    int posX;
    int posY;
    Color color;
    bool alive;
} Entity;

Entity *buildEntities() {
    EntityType entityType = SHIP;

    Entity *entities = (Entity *)malloc((N_ENEMIES + 2) * sizeof(Entity));
    // Setup player ship
    entities[0].type = entityType;
    entities[0].width = SHIP_WIDTH;
    entities[0].height = SHIP_HEIGHT;
    entities[0].posX = (SCREEN_WIDTH - SHIP_WIDTH)/2;
    entities[0].posY = SHIP_POS_Y;
    entities[0].color = RAYWHITE;
    entities[0].alive = true;

    int enemiesXOffSet = (SCREEN_WIDTH - ENEMIES_PER_ROW*(ENEMIES_WIDTH + ENEMIES_GAP_X))/2;
    for (int i = 0; i < N_ENEMIES; ++i) {
        if (i % N_ENEMIES_ROWS == 0) entityType++;
        entities[i+2].type = entityType;
        entities[i+2].width = ENEMIES_WIDTH;
        entities[i+2].height = ENEMIES_WIDTH;
        entities[i+2].posX = enemiesXOffSet + (ENEMIES_WIDTH + ENEMIES_GAP_X)*(i%ENEMIES_PER_ROW);
        entities[i+2].posY = ENEMIES_OFFSET_Y + (ENEMIES_WIDTH + ENEMIES_GAP_Y)*(i/ENEMIES_PER_ROW);
        entities[i+2].color = DARKGREEN;
        entities[i+2].alive = true;
    }

    // Setup Enemy ship
    entities[1].type = entityType;
    entities[1].width = ENEMY_SHIP_WIDTH;
    entities[1].height = ENEMY_SHIP_HEIGHT;
    entities[1].posX = (SCREEN_WIDTH - ENEMY_SHIP_WIDTH)/2;
    entities[1].posY = ENEMY_SHIP_POS_Y;
    entities[1].color = PURPLE;
    entities[1].alive = false;

    return entities;
}

void drawEntites(Entity *entities) {
    for (int i = 0; i < N_ENEMIES + 2; ++i) {
        if (entities[i].alive)
            DrawRectangle(
                entities[i].posX,
                entities[i].posY,
                entities[i].width,
                entities[i].height,
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

int main(void) {
    srand(time(NULL));
    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "Space Invaders clone using raylib"
    );

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    DisableCursor();
    Entity *entities = buildEntities();
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

        if (IsKeyDown(KEY_LEFT)) entities[0].posX -= 10;
        if (IsKeyDown(KEY_RIGHT)) entities[0].posX += 10;

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