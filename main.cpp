#include "LevelA.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1400,
              SCREEN_HEIGHT    = 800,
              FPS              = 120,
              NUMBER_OF_LEVELS = 2;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

std::vector<Entity*> gLifeHearts = {};

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;

Menu  *gMenuScreen    = nullptr;

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
    if (gCurrentScene != nullptr) gCurrentScene->shutdown();

    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gMenuScreen = new Menu(ORIGIN, "#222831ff");
    gLevelA = new LevelA(ORIGIN, "#37675fff");
   
    gLevels.push_back(gMenuScreen);
    gLevels.push_back(gLevelA);


    switchToScene(gLevels[0]);

    // Camera2D cam = gCurrentScene->getState().camera;
    // Vector2 topLeft = {
    //     cam.target.x - cam.offset.x,
    //     cam.target.y - cam.offset.y
    // };

    for (int i = 0; i < 3; i++)
    {
        // Vector2 heartPos = { topLeft.x + 40.0f + (i * 55.0f), topLeft.y + 40.0f };
        Entity *heart = new Entity(
            ORIGIN,
            { 42.0f, 42.0f },
            "assets/life_heart.png",
            BLOCK
        );
        heart->setColliderDimensions({ 0.0f, 0.0f });
        heart->deactivate();
        gLifeHearts.push_back(heart);
    }

    

    SetTargetFPS(FPS);

}

void processInput() 
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    if (gCurrentScene == gMenuScreen) {

        if (IsKeyPressed(KEY_ENTER)) gMenuScreen->setGameCondition();
    }

    GameState state = gCurrentScene->getState();

    Entity *controlledEntity = (state.drivingCar && state.car != nullptr)
        ? state.car
        : state.witch;

    if (controlledEntity == nullptr) return;

    controlledEntity->resetMovement();

    if (state.drivingCar && state.car != nullptr)
    {
        if      (IsKeyDown(KEY_W)) controlledEntity->moveUp();
        else if (IsKeyDown(KEY_S)) controlledEntity->moveDown();

        if (IsKeyPressed(KEY_SPACE) && gLevelA != nullptr)
            gLevelA->exitCar();
    }
    else
    {
        if      (IsKeyDown(KEY_A)) state.witch->moveLeft();
        else if (IsKeyDown(KEY_D)) state.witch->moveRight();

        if      (IsKeyDown(KEY_W)) state.witch->moveUp();
        else if (IsKeyDown(KEY_S)) state.witch->moveDown();

        if (IsKeyPressed(KEY_J))
            state.witch->attack();
    }

    if (GetLength(controlledEntity->getMovement()) > 1.0f) 
        controlledEntity->normaliseMovement();

    // if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
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
    GameState state = gCurrentScene->getState();
    if (gCurrentScene != gMenuScreen)
    {
        Camera2D cam = state.camera;
        Vector2 topLeft = {
            cam.target.x - cam.offset.x,
            cam.target.y - cam.offset.y
        };

        for (size_t i = 0; i < gLifeHearts.size(); i++)
        {
            Vector2 heartPos = { topLeft.x + 40.0f + (float) (i) * 55.0f,
                                topLeft.y + 40.0f };
            gLifeHearts[i]->setPosition(heartPos);

            if ((int) (i) < state.livesRemaining)
                gLifeHearts[i]->activate();
            else
                gLifeHearts[i]->deactivate();
        }
    }

    BeginDrawing();
    BeginMode2D (gCurrentScene->getState().camera);

    gCurrentScene->render();

    for (Entity *heart : gLifeHearts) heart->render();

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

            if (id == 0 && gLevelA != nullptr)
            {
                gLevelA->resetLivesToMax();
                for (Entity *heart : gLifeHearts) if (heart != nullptr) heart->deactivate();
            }

            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}
