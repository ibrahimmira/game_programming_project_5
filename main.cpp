#include "LevelA.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1400,
              SCREEN_HEIGHT    = 800,
              FPS              = 120,
              NUMBER_OF_LEVELS = 2;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
LevelA *gLevelA = nullptr;
std::vector<Scene*> gLevels = {};

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gLevelA = new LevelA(ORIGIN, "#37675fff");
   

    gLevels.push_back(gLevelA);


    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);

}

void processInput() 
{
    gCurrentScene->getState().witch->resetMovement();

    if      (IsKeyDown(KEY_A)) gCurrentScene->getState().witch->moveLeft();
    else if (IsKeyDown(KEY_D)) gCurrentScene->getState().witch->moveRight();

    if      (IsKeyDown(KEY_W)) gCurrentScene->getState().witch->moveUp();
    else if (IsKeyDown(KEY_S)) gCurrentScene->getState().witch->moveDown();

    if (GetLength(gCurrentScene->getState().witch->getMovement()) > 1.0f) 
        gCurrentScene->getState().witch->normaliseMovement();

    if (IsKeyPressed(KEY_J))
        gCurrentScene->getState().witch->attack();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D (gCurrentScene->getState().camera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID >= 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}
