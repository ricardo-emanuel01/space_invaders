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
# define EXIT_MESSAGE_FONT_SIZE 80
# define MENU_BANNER_WIDTH 600
# define MENU_BANNER_HEIGHT 400
# define MENU_BANNER_POS_X ((SCREEN_WIDTH-MENU_BANNER_WIDTH)/2)
# define MENU_BANNER_POS_Y ((SCREEN_HEIGHT-MENU_BANNER_HEIGHT)/2)
# define BULLET_SPEED 20 // pixels/frame
# define N_BULLETS 10000
# define BULLET_WIDTH 4
# define BULLET_HEIGHT 32
# define N_POWER_UPS 10
# define ENTITIES_ARRAY_SIZE (2+N_ENEMIES+N_BULLETS+N_POWER_UPS)
# define FIRST_IDX_BULLETS (2+N_ENEMIES)
# define FIRST_IDX_POWER_UPS (2+N_ENEMIES+N_BULLETS)
# define SCREEN_LIMIT 250
# define SCREEN_LIMIT_LEFT SCREEN_LIMIT
# define SCREEN_LIMIT_RIGHT (SCREEN_WIDTH-SCREEN_LIMIT)
# define HORDE_SCREEN_LIMIT 400
# define HORDE_LIMIT_LEFT HORDE_SCREEN_LIMIT
# define HORDE_LIMIT_RIGHT (SCREEN_WIDTH-HORDE_SCREEN_LIMIT)
# define WALL_COLISION_DELAY 1.0f
# define SHIP_TEXTURE_WIDTH 16.0f
# define SHIP_TEXTURE_HEIGHT 12.0f
# define N_SHIP_FRAMES 2
# define ENEMY_SLOW_TEXTURE_WIDTH 16.0f
# define ENEMY_SLOW_TEXTURE_HEIGHT ENEMY_SLOW_TEXTURE_WIDTH
# define N_ENEMY_SLOW_FRAMES 4
# define ENEMY_FAST_TEXTURE_WIDTH ENEMY_SLOW_TEXTURE_WIDTH
# define ENEMY_FAST_TEXTURE_HEIGHT ENEMY_SLOW_TEXTURE_WIDTH
# define N_ENEMY_FAST_FRAMES 4
# define ENEMY_SHIP_TEXTURE_WIDTH ENEMY_SLOW_TEXTURE_WIDTH
# define ENEMY_SHIP_TEXTURE_HEIGHT ENEMY_SLOW_TEXTURE_WIDTH
# define N_ENEMY_SHIP_FRAMES 2
# define BULLET_TEXTURE_WIDTH 4.0f
# define BULLET_TEXTURE_HEIGHT 8.0f
# define N_BULLET_FRAMES 1
# define REGULAR_BUTTON_SIZE 80
# define SELECT_BUTTON_SIZE_INCREMENT 20
# define FAST_SHOT_DROP_RATE 30
# define POWER_UP_WIDTH 25
# define POWER_UP_HEIGHT POWER_UP_WIDTH
# define POWER_UP_TEXTURE_WIDTH 18
# define POWER_UP_TEXTURE_HEIGHT POWER_UP_TEXTURE_WIDTH
# define N_POWER_FRAMES 1
# define POWER_UP_DURATION 2.0
# define ENEMY_SLOW_POINTS 350
# define ENEMY_FAST_POINTS 500
# define ENEMY_INITIAL_VELOCITY_X 2.0f
# define INCREMENT_ENEMY_VELOCITY_X_PER_LEVEL 0.5f
# define SLOW_ENEMY_DELAY_TO_FIRE 1.0f
# define FAST_ENEMY_DELAY_TO_FIRE 0.5f
# define SHIP_REGULAR_DELAY_TO_FIRE 0.5f
# define SHIP_BUFFED_DELAY_TO_FIRE 0.1f
# define HORDE_STEP_Y_AXIS 50.0f
# define SHIP_X_MOVEMENT 10.0f
# define PATH_BG_MUSIC "resources/sounds/BGMusic.ogg"
# define PATH_ALIEN_EXPLOSION_FX "resources/sounds/alienExplosionFX.ogg"
# define PATH_SHIP_EXPLOSION_FX "resources/sounds/shipExplosionFX.ogg"
# define PATH_ALIEN_FIRE_FX "resources/sounds/alienFireFX.ogg"
# define PATH_SHIP_FIRE_FX "resources/sounds/shipFireFX.ogg"
# define PATH_POWER_UP_FX "resources/sounds/powerUpFX.ogg"
# define PATH_VICTORY_FX "resources/sounds/victoryFX.ogg"
# define PATH_SHIP_TEX "resources/textures/shipTex.png"
# define PATH_ENEMY_SLOW_TEX "resources/textures/enemySlowTex.png"
# define PATH_ENEMY_FAST_TEX "resources/textures/enemyFastTex.png"
# define PATH_SINGLE_BULLET_TEX "resources/textures/singleBulletTex.png"
# define PATH_FAST_SHOT_POWER_UP_TEX "resources/textures/fastShotPowerUpTex.png"


const char exitMessage[] = "Are you sure you want to exit game? [Y/N]";
const float delayToFire[] = {
    SHIP_REGULAR_DELAY_TO_FIRE,
    SLOW_ENEMY_DELAY_TO_FIRE,
    FAST_ENEMY_DELAY_TO_FIRE,
    SHIP_BUFFED_DELAY_TO_FIRE
};

typedef enum GameState {
    MENU,
    PLAYING,
    WIN,
    LOSE,
} GameState;

typedef enum MenuItem {
    RESTART,
    START,
    QUIT,
} MenuItem;

typedef enum EntityType {
    SHIP,
    ENEMY_SHIP,
    ENEMY_SLOW,
    ENEMY_FAST,
    BULLET,
    POWER_UP,
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
    double powerUpTimeOut;
    // Used when there is an active power up and the player pauses the game
    double powerUpRemainingTime;
    float wallCollisionDelay;
    float incrementPerLevel;
    int firstAlive;
    Entity *entities;
    GameState gameState;
    MenuItem menuItem;
    Music BGMusic;
    Sound shipFire;
    Sound alienFire;
    Sound shipExplosion;
    Sound alienExplosion;
    Sound powerUp;
    Sound victory;
    Texture2D shipSingleShot;
    Texture2D singleBullet;
    Texture2D enemyFast;
    Texture2D enemySlow;
    Texture2D fastShotPowerUp;
    int startButtonSize;
    int quitButtonSize;
    int restartButtonSize;
    bool activePowerUp;
} GameData;

void initShip(Entity *entities) {
    entities[SHIP].type = SHIP;
    entities[SHIP].bounds.x = (SCREEN_WIDTH - SHIP_WIDTH)/2;
    entities[SHIP].bounds.y = SHIP_POS_Y;
    entities[SHIP].bounds.width = SHIP_WIDTH;
    entities[SHIP].bounds.height = SHIP_HEIGHT;
    entities[SHIP].animationFrame.nFrames = N_SHIP_FRAMES;
    entities[SHIP].animationFrame.currentFrame = 1;
    entities[SHIP].animationFrame.frameBounds.x = SHIP_TEXTURE_WIDTH;
    entities[SHIP].animationFrame.frameBounds.y = 0.0f;
    entities[SHIP].animationFrame.frameBounds.width = SHIP_TEXTURE_WIDTH;
    entities[SHIP].animationFrame.frameBounds.height = SHIP_TEXTURE_HEIGHT;
    entities[SHIP].velocity.x = 0.0f;
    entities[SHIP].velocity.y = 0.0f;
    entities[SHIP].color = WHITE;
    entities[SHIP].alive = true;
    entities[SHIP].delayToFire = delayToFire[SHIP];
    entities[SHIP].canFire = true;
    entities[SHIP].lastShotTime = GetTime();
    entities[SHIP].points = 0;
}

void initHorde(Entity *entities) {
    int enemiesXOffSet = (SCREEN_WIDTH - ENEMIES_PER_ROW*(ENEMIES_WIDTH + ENEMIES_GAP_X))/2;
    EntityType entityType = ENEMY_SHIP;
    for (int i = 0; i < N_ENEMIES; ++i) {
        if (i % (N_ENEMIES/2) == 0) entityType++;
        entities[i+2].type = entityType;
        entities[i+2].bounds.x = enemiesXOffSet + (ENEMIES_WIDTH + ENEMIES_GAP_X)*(i%ENEMIES_PER_ROW);
        entities[i+2].bounds.y = ENEMIES_OFFSET_Y + (ENEMIES_WIDTH + ENEMIES_GAP_Y)*(i/ENEMIES_PER_ROW);
        entities[i+2].bounds.width = ENEMIES_WIDTH;
        entities[i+2].bounds.height = ENEMIES_HEIGHT;
        entities[i+2].animationFrame.nFrames = N_ENEMY_SLOW_FRAMES;
        entities[i+2].animationFrame.currentFrame = rand() % N_ENEMY_SLOW_FRAMES;
        entities[i+2].animationFrame.frameBounds.x = 0.0f;
        entities[i+2].animationFrame.frameBounds.y = 0.0f;
        entities[i+2].animationFrame.frameBounds.width = ENEMY_SLOW_TEXTURE_WIDTH;
        entities[i+2].animationFrame.frameBounds.height = ENEMY_SLOW_TEXTURE_HEIGHT;
        entities[i+2].velocity.x = ENEMY_INITIAL_VELOCITY_X;
        entities[i+2].velocity.y = 0.0f;
        entities[i+2].color = WHITE;
        entities[i+2].alive = true;
        entities[i+2].delayToFire = delayToFire[entityType];
        entities[i+2].canFire = true;
        entities[i+2].lastShotTime = GetTime();
        if (entityType == ENEMY_FAST) {
            entities[i+2].points = ENEMY_FAST_POINTS;
        } else if (entityType == ENEMY_SLOW) {
            entities[i+2].points = ENEMY_SLOW_POINTS;
        }
    }
}

void initEnemyShip(Entity *entities) {
    entities[ENEMY_SHIP].type = ENEMY_SHIP;
    entities[ENEMY_SHIP].bounds.x = (SCREEN_WIDTH - ENEMY_SHIP_WIDTH)/2;
    entities[ENEMY_SHIP].bounds.y = ENEMY_SHIP_POS_Y;
    entities[ENEMY_SHIP].bounds.width = ENEMY_SHIP_WIDTH;
    entities[ENEMY_SHIP].bounds.height = ENEMY_SHIP_HEIGHT;
    entities[ENEMY_SHIP].animationFrame.nFrames = N_ENEMY_SHIP_FRAMES;
    entities[ENEMY_SHIP].animationFrame.currentFrame = 0;
    entities[ENEMY_SHIP].animationFrame.frameBounds.x = 0.0f;
    entities[ENEMY_SHIP].animationFrame.frameBounds.y = 0.0f;
    entities[ENEMY_SHIP].animationFrame.frameBounds.width = ENEMY_SHIP_TEXTURE_WIDTH;
    entities[ENEMY_SHIP].animationFrame.frameBounds.height = ENEMY_SHIP_TEXTURE_HEIGHT;
    entities[ENEMY_SHIP].velocity.x = 5.0f;
    entities[ENEMY_SHIP].velocity.y = 0.0f;
    entities[ENEMY_SHIP].color = WHITE;
    entities[ENEMY_SHIP].alive = false;
    entities[ENEMY_SHIP].delayToFire = delayToFire[ENEMY_SHIP];
    entities[ENEMY_SHIP].canFire = true;
    entities[ENEMY_SHIP].lastShotTime = GetTime();
    entities[ENEMY_SHIP].points = 5000;
}

void initBullets(Entity *entities) {
    for (int i = FIRST_IDX_BULLETS; i < FIRST_IDX_POWER_UPS; ++i) {
        entities[i].type = BULLET;
        entities[i].bounds.width = BULLET_WIDTH;
        entities[i].bounds.height = BULLET_HEIGHT;
        entities[i].animationFrame.nFrames = N_BULLET_FRAMES;
        entities[i].animationFrame.currentFrame = 0;
        entities[i].animationFrame.frameBounds.x = 0.0f;
        entities[i].animationFrame.frameBounds.y = 0.0f;
        entities[i].animationFrame.frameBounds.width = BULLET_TEXTURE_WIDTH;
        entities[i].animationFrame.frameBounds.height = BULLET_TEXTURE_HEIGHT;
        entities[i].alive = false;
        entities[i].color = RAYWHITE;
    }
}

void initPowerUps(Entity *entities) {
    for (int i = FIRST_IDX_POWER_UPS; i < ENTITIES_ARRAY_SIZE; ++i) {
        entities[i].type = POWER_UP;
        entities[i].bounds.width = POWER_UP_WIDTH;
        entities[i].bounds.height = POWER_UP_HEIGHT;
        entities[i].alive = false;
        entities[i].velocity.y = BULLET_SPEED;
        entities[i].points = 0;
        entities[i].animationFrame.nFrames = N_POWER_FRAMES;
        entities[i].animationFrame.currentFrame = 0;
        entities[i].animationFrame.frameBounds.x = 0.0f;
        entities[i].animationFrame.frameBounds.y = 0.0f;
        entities[i].animationFrame.frameBounds.width = POWER_UP_TEXTURE_WIDTH;
        entities[i].animationFrame.frameBounds.height = POWER_UP_TEXTURE_HEIGHT;
    }
}

Entity *buildEntities() {
    Entity *entities = (Entity *)malloc((ENTITIES_ARRAY_SIZE) * sizeof(Entity));

    initShip(entities);
    initHorde(entities);
    initBullets(entities);
    initPowerUps(entities);

    return entities;
}

GameData initGame() {
    srand(time(NULL));

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
        .incrementPerLevel=INCREMENT_ENEMY_VELOCITY_X_PER_LEVEL,
        .firstAlive=2,
        .entities=buildEntities(),
        .gameState=MENU,
        .menuItem=START,
        .BGMusic=LoadMusicStream(PATH_BG_MUSIC),
        .shipFire=LoadSound(PATH_SHIP_FIRE_FX),
        .alienFire=LoadSound(PATH_ALIEN_FIRE_FX),
        .shipExplosion=LoadSound(PATH_SHIP_EXPLOSION_FX),
        .alienExplosion=LoadSound(PATH_ALIEN_EXPLOSION_FX),
        .powerUp=LoadSound(PATH_POWER_UP_FX),
        .victory=LoadSound(PATH_VICTORY_FX),
        .shipSingleShot=LoadTexture(PATH_SHIP_TEX),
        .enemySlow=LoadTexture(PATH_ENEMY_SLOW_TEX),
        .enemyFast=LoadTexture(PATH_ENEMY_FAST_TEX),
        .singleBullet=LoadTexture(PATH_SINGLE_BULLET_TEX),
        .fastShotPowerUp=LoadTexture(PATH_FAST_SHOT_POWER_UP_TEX),
        .startButtonSize=REGULAR_BUTTON_SIZE+SELECT_BUTTON_SIZE_INCREMENT,
        .quitButtonSize=REGULAR_BUTTON_SIZE,
        .restartButtonSize=REGULAR_BUTTON_SIZE+SELECT_BUTTON_SIZE_INCREMENT,
        .activePowerUp=false,
    };

    gameData.BGMusic.looping = true;

    PlayMusicStream(gameData.BGMusic);

    return gameData;
}

void rebootGame(GameData *gameData) {
    initShip(gameData->entities);
    initHorde(gameData->entities);
    initBullets(gameData->entities);
    initPowerUps(gameData->entities);

    gameData->exitWindow = false,
    gameData->canCollideOnWall = true,
    gameData->timeLastWallCollision = GetTime(),
    gameData->wallCollisionDelay = WALL_COLISION_DELAY,
    gameData->incrementPerLevel = INCREMENT_ENEMY_VELOCITY_X_PER_LEVEL,
    gameData->firstAlive = 2,
    gameData->gameState = PLAYING;
    gameData->menuItem = START;
    gameData->activePowerUp = false;
    PlayMusicStream(gameData->BGMusic);
}

void closeGame(GameData *gameData) {
    UnloadMusicStream(gameData->BGMusic);
    UnloadSound(gameData->shipFire);
    UnloadSound(gameData->shipExplosion);
    UnloadSound(gameData->alienFire);
    UnloadSound(gameData->alienExplosion);
    UnloadSound(gameData->powerUp);
    UnloadSound(gameData->victory);
    UnloadTexture(gameData->shipSingleShot);
    UnloadTexture(gameData->enemyFast);
    UnloadTexture(gameData->enemySlow);
    UnloadTexture(gameData->singleBullet);
    UnloadTexture(gameData->fastShotPowerUp);
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

void drawMenu(GameData *gameData) {
    Rectangle banner = {
        .x=MENU_BANNER_POS_X,
        .y=MENU_BANNER_POS_Y,
        .width=MENU_BANNER_WIDTH,
        .height=MENU_BANNER_HEIGHT,
    };

    switch (gameData->gameState) {
        case WIN:
        case LOSE:
        {
            int restartButtonSize = MeasureText("RESTART", gameData->restartButtonSize);
            int quitButtonSize = MeasureText("QUIT", gameData->quitButtonSize);
            DrawRectangleRec(banner, RAYWHITE);
            DrawText(
                "RESTART",
                (SCREEN_WIDTH-restartButtonSize)/2,
                SCREEN_HEIGHT/2 - 100,
                gameData->restartButtonSize,
                BLACK
            );
            DrawText(
                "QUIT",
                (SCREEN_WIDTH-quitButtonSize)/2,
                SCREEN_HEIGHT/2,
                gameData->quitButtonSize,
                BLACK
            );
        } break;
        default:
        {
            int startButtonSize = MeasureText("START", gameData->startButtonSize);
            int quitButtonSize = MeasureText("QUIT", gameData->quitButtonSize);
            DrawRectangleRec(banner, RAYWHITE);
            DrawText(
                "START",
                (SCREEN_WIDTH-startButtonSize)/2,
                SCREEN_HEIGHT/2 - 100,
                gameData->startButtonSize,
                BLACK
            );
            DrawText(
                "QUIT",
                (SCREEN_WIDTH-quitButtonSize)/2,
                SCREEN_HEIGHT/2,
                gameData->quitButtonSize,
                BLACK
            );
        } break;
    }
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
                case POWER_UP:
                {
                    currentTexture = gameData->fastShotPowerUp;
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

void drawVictory() {
    int victoryMessage = MeasureText("VICTORY", 150);
    DrawText(
        "VICTORY",
        (SCREEN_WIDTH-victoryMessage)/2,
        100,
        150,
        RAYWHITE
    );
}

void drawGame(GameData *gameData) {
    ClearBackground(BLACK);
    DrawFPS(10, 10);

    switch (gameData->gameState) {
        case MENU:
        {
            drawEntities(gameData);
            drawMenu(gameData);
        } break;
        case PLAYING:
        {
            drawEntities(gameData);
        } break;
        case WIN:
        {
            drawVictory();
        }
        case LOSE:
        {
            drawEntities(gameData);
            drawMenu(gameData);
        } break;
        // While WIN and LOSE are not implemented
        default:
        {
            drawEntities(gameData);
            drawExitMessage();
        } break;
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

        entities[firstBulletAvailable].bounds.x = entities[shooterIdx].bounds.x + (entities[shooterIdx].bounds.width-BULLET_WIDTH)/2;
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

void generatePowerUp(GameData *gameData, int srcIndex) {
    int dropCheck = rand() % 100;
    Entity *entities = gameData->entities;

    if (dropCheck < FAST_SHOT_DROP_RATE) {
        for (int i = FIRST_IDX_POWER_UPS; i < ENTITIES_ARRAY_SIZE; ++i) {
            if (entities[i].alive) continue;
            entities[i].bounds.x = entities[srcIndex].bounds.x + (entities[srcIndex].bounds.width-entities[i].bounds.width)/2;
            entities[i].bounds.y = entities[srcIndex].bounds.y + entities[srcIndex].bounds.height;
            entities[i].alive = true;
            entities[i].shotSrc = entities[srcIndex].type;
            break;
        }
    }
}

// Implements a naive collision detection
void detectCollisions(GameData *gameData) {
    Entity *entities = gameData->entities;
    for (int i = 0; i < FIRST_IDX_BULLETS; ++i) {
        if (!entities[i].alive) continue;

        float leftEdgeEntity = entities[i].bounds.x;
        float rightEdgeEntity = leftEdgeEntity + entities[i].bounds.width;
        float topEdgeEntity = entities[i].bounds.y;
        float bottomEdgeEntity = topEdgeEntity + entities[i].bounds.height;

        for (int current_collider = FIRST_IDX_BULLETS; current_collider < ENTITIES_ARRAY_SIZE; ++current_collider) {
            if (!entities[current_collider].alive) continue;
            if (entities[current_collider].shotSrc == entities[i].type) continue;
            if (
                (entities[i].type >= ENEMY_SHIP && entities[i].type <= ENEMY_FAST) &&
                (entities[current_collider].shotSrc >= ENEMY_SHIP && entities[current_collider].shotSrc <= ENEMY_FAST)
            ) continue;

            float leftEdgeCollider = entities[current_collider].bounds.x;
            float topEdgeCollider = entities[current_collider].bounds.y;
            float rightEdgeCollider = leftEdgeCollider + entities[current_collider].bounds.width;
            float bottomEdgeCollider = topEdgeCollider + entities[current_collider].bounds.height;

            if (
                (rightEdgeEntity >= leftEdgeCollider) &&
                (leftEdgeEntity <= rightEdgeCollider) &&
                (bottomEdgeEntity >= topEdgeCollider) &&
                (topEdgeEntity <= bottomEdgeCollider)
            ) {
                entities[i].alive = false;
                entities[current_collider].alive = false;

                if (i == gameData->firstAlive) {
                    for (int j = i+1; j < FIRST_IDX_BULLETS && !entities[gameData->firstAlive].alive; ++j) {
                        if (entities[j].alive) gameData->firstAlive = j;
                    }
                    if (!entities[gameData->firstAlive].alive) {
                        gameData->gameState = WIN;
                        gameData->menuItem = RESTART;
                        StopMusicStream(gameData->BGMusic);
                        PlaySound(gameData->victory);
                    }
                }

                if (entities[i].type != SHIP) {
                    entities[SHIP].points += entities[i].points;
                    generatePowerUp(gameData, i);
                    PlaySound(gameData->alienExplosion);
                } else {
                    switch (entities[current_collider].type) {
                        case POWER_UP:
                        {
                            entities[SHIP].alive = true;
                            entities[SHIP].delayToFire = 0.1f;
                            gameData->powerUpTimeOut = GetTime() + POWER_UP_DURATION;
                            gameData->activePowerUp = true;
                            PlaySound(gameData->powerUp);
                        } break;
                        case BULLET:
                        {
                            gameData->gameState = LOSE;
                            gameData->menuItem = RESTART;
                            PlaySound(gameData->shipExplosion);
                        } break;
                        default: break;
                    }
                } 
            }
        }
    }
}

void enemyAI(GameData *gameData) {
    int enemyIndex = rand() % (N_ENEMIES*15);
    if (enemyIndex > ENEMY_SHIP && enemyIndex < FIRST_IDX_BULLETS)
        fire(gameData, enemyIndex);
}

void updateGame(GameData *gameData) {
    UpdateMusicStream(gameData->BGMusic);
    switch (gameData->gameState) {
        case PLAYING:
        {
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
                } else if (entities[i].type != BULLET && entities[i].type != POWER_UP) {
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
                    entities[i].velocity.y += HORDE_STEP_Y_AXIS;
                }

                entities[i].bounds.x += entities[i].velocity.x;
                entities[i].bounds.y += entities[i].velocity.y;

                if (entities[i].type == SHIP) {
                    entities[i].velocity.x = 0.0f;
                }

                if (entities[i].type != BULLET && entities[i].type != POWER_UP) {
                    entities[i].velocity.y = 0.0f;
                    double currentTime = GetTime();
                    if (currentTime - entities[i].lastShotTime > entities[i].delayToFire) {
                        entities[i].canFire = true;
                    }
                } else if (entities[i].bounds.y < 0.0f || entities[i].bounds.y > SCREEN_HEIGHT) {
                    entities[i].alive = false;
                }
                
                if (GetTime()-gameData->timeLastWallCollision > gameData->wallCollisionDelay) {
                    gameData->canCollideOnWall = true;
                }
                if (GetTime()-gameData->powerUpTimeOut > 0.0) {
                    gameData->activePowerUp = false;
                    entities[SHIP].delayToFire = SHIP_REGULAR_DELAY_TO_FIRE;
                }
            }
        } break;
        default: break;
    }
}

void processInput(GameData *gameData) {
    switch (gameData->gameState) {
        case MENU:
        {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) {
                if (gameData->menuItem == START) {
                    gameData->startButtonSize -= SELECT_BUTTON_SIZE_INCREMENT;
                    gameData->quitButtonSize += SELECT_BUTTON_SIZE_INCREMENT;
                    if (gameData->startButtonSize < REGULAR_BUTTON_SIZE)
                        gameData->startButtonSize = REGULAR_BUTTON_SIZE;
                    if (gameData->quitButtonSize > REGULAR_BUTTON_SIZE + SELECT_BUTTON_SIZE_INCREMENT)
                        gameData->quitButtonSize = REGULAR_BUTTON_SIZE + SELECT_BUTTON_SIZE_INCREMENT;

                    gameData->menuItem = QUIT;
                } else {
                    gameData->startButtonSize += SELECT_BUTTON_SIZE_INCREMENT;
                    gameData->quitButtonSize -= SELECT_BUTTON_SIZE_INCREMENT;
                    if (gameData->startButtonSize > REGULAR_BUTTON_SIZE + SELECT_BUTTON_SIZE_INCREMENT)
                        gameData->startButtonSize = REGULAR_BUTTON_SIZE + SELECT_BUTTON_SIZE_INCREMENT;
                    if (gameData->quitButtonSize < REGULAR_BUTTON_SIZE)
                        gameData->quitButtonSize = REGULAR_BUTTON_SIZE;

                    gameData->menuItem = START;
                }
            }

            if (IsKeyPressed(KEY_SPACE)) {
                if (gameData->menuItem == START) {
                    if (gameData->activePowerUp) {
                        gameData->powerUpTimeOut = GetTime() + gameData->powerUpRemainingTime;
                    }
                    gameData->gameState = PLAYING;
                } else gameData->exitWindow = true;
            }
        } break;
        case PLAYING:
        {
            if (WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)) {
                if (gameData->activePowerUp) {
                    gameData->powerUpRemainingTime = gameData->powerUpTimeOut - GetTime();
                }
                gameData->gameState = MENU;
            }
            if (IsKeyDown(KEY_LEFT)) gameData->entities[0].velocity.x = -SHIP_X_MOVEMENT;
            if (IsKeyDown(KEY_RIGHT)) gameData->entities[0].velocity.x = SHIP_X_MOVEMENT;
            if (IsKeyPressed(KEY_SPACE)) fire(gameData, SHIP);
        } break;
        case WIN:
        case LOSE:
        {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) {
                if (gameData->menuItem == RESTART) {
                    gameData->restartButtonSize -= SELECT_BUTTON_SIZE_INCREMENT;
                    gameData->quitButtonSize += SELECT_BUTTON_SIZE_INCREMENT;
                    if (gameData->restartButtonSize < REGULAR_BUTTON_SIZE)
                        gameData->restartButtonSize = REGULAR_BUTTON_SIZE;
                    if (gameData->quitButtonSize > REGULAR_BUTTON_SIZE + SELECT_BUTTON_SIZE_INCREMENT)
                        gameData->quitButtonSize = REGULAR_BUTTON_SIZE + SELECT_BUTTON_SIZE_INCREMENT;

                    gameData->menuItem = QUIT;
                } else {
                    gameData->restartButtonSize += SELECT_BUTTON_SIZE_INCREMENT;
                    gameData->quitButtonSize -= SELECT_BUTTON_SIZE_INCREMENT;
                    if (gameData->restartButtonSize > REGULAR_BUTTON_SIZE + SELECT_BUTTON_SIZE_INCREMENT)
                        gameData->restartButtonSize = REGULAR_BUTTON_SIZE + SELECT_BUTTON_SIZE_INCREMENT;
                    if (gameData->quitButtonSize < REGULAR_BUTTON_SIZE)
                        gameData->quitButtonSize = REGULAR_BUTTON_SIZE;

                    gameData->menuItem = RESTART;
                }
            }

            if (IsKeyPressed(KEY_SPACE)) {
                if (gameData->menuItem == RESTART) {
                    gameData->gameState = PLAYING;
                    rebootGame(gameData);
                } else gameData->exitWindow = true;
            }
        } break;
        // While WIN and LOSE are not implmemented
        default:
        {
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) {
                if (gameData->menuItem == START) {
                    gameData->startButtonSize -= SELECT_BUTTON_SIZE_INCREMENT;
                    gameData->quitButtonSize += SELECT_BUTTON_SIZE_INCREMENT;
                    gameData->menuItem = QUIT;
                } else {
                    gameData->startButtonSize += SELECT_BUTTON_SIZE_INCREMENT;
                    gameData->quitButtonSize -= SELECT_BUTTON_SIZE_INCREMENT;
                    gameData->menuItem = START;
                }
            }

            if (IsKeyDown(KEY_ENTER)) {
                if (gameData->menuItem == START) {
                    gameData->gameState = PLAYING;
                } else gameData->exitWindow = true;
            }
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