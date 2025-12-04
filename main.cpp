#include "ShaderProgram.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1400,
              SCREEN_HEIGHT    = 800,
              FPS              = 120,
              NUMBER_OF_LEVELS = 8;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

std::vector<Entity*> gLifeHearts = {};

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;

Menu  *gMenuScreen    = nullptr;
Controls *gControlsScreen = nullptr;
GameInstructions *gInstructionsLevelA = nullptr;
LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelCTransition *gLevelCTransition = nullptr;
LevelC *gLevelC = nullptr;
WinMessage *gWinMessage = nullptr;
LoseMessage *gLoseMessage = nullptr;

std::vector<Scene*> gLevels = {};

Effects *gEffects = nullptr;

ShaderProgram gShader;
Vector2 gLightPosition = { 0.0f, 0.0f };

int applyShader = 1;
bool flashlightOn = false;
bool lightActive = false;
bool wasDrivingCar = false;
int gSharedLives = -1;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    if (gCurrentScene != nullptr) {
        gSharedLives = gCurrentScene->getState().livesRemaining;
        gCurrentScene->shutdown();
    }

    gCurrentScene = scene;
    gCurrentScene->initialise();
    bool resetLives = (gCurrentScene == gMenuScreen ||
                       gCurrentScene == gWinMessage ||
                       gCurrentScene == gLoseMessage);

    if (resetLives)
    {
        int maxLives = gCurrentScene->getState().maxLives;
        gCurrentScene->setLivesRemaining(maxLives);
        gSharedLives = maxLives;
    }
    else if (gSharedLives >= 0)
    {
        gCurrentScene->setLivesRemaining(gSharedLives);
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gShader.load("CS3113/shaders/vertex.glsl", "CS3113/shaders/fragment.glsl");

    gMenuScreen = new Menu(ORIGIN, "#222831ff");
    gControlsScreen = new Controls(ORIGIN, "#222831ff");
    gInstructionsLevelA = new GameInstructions(ORIGIN, "#222831ff");
    gLevelA = new LevelA(ORIGIN, "#37675fff");
    gLevelB = new LevelB(ORIGIN, "#37675fff");
    gLevelCTransition = new LevelCTransition(ORIGIN, "#222831ff");
    gLevelC = new LevelC(ORIGIN, "#37675fff");
    gWinMessage = new WinMessage(ORIGIN, "#222831ff");
    gLoseMessage = new LoseMessage(ORIGIN, "#222831ff");

    gLevels.push_back(gMenuScreen); // Scene 0
    gLevels.push_back(gControlsScreen); // Scene 1
    gLevels.push_back(gInstructionsLevelA); // Scene 2
    gLevels.push_back(gLevelA); // Scene 3
    gLevels.push_back(gLevelB); // Scene 4
    gLevels.push_back(gLevelCTransition); // Scene 5
    gLevels.push_back(gLevelC); // Scene 6
    gLevels.push_back(gWinMessage); // Scene 7
    gLevels.push_back(gLoseMessage); // Scene 8

    switchToScene(gLevels[0]);
    gEffects = new Effects(ORIGIN, (float) SCREEN_WIDTH * 1.25f, (float) SCREEN_HEIGHT * 2.25f);

    gEffects->start(FADEIN);
    gEffects->setEffectSpeed(0.33f);

    for (int i = 0; i < 3; i++)
    {
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
        if (IsKeyPressed(KEY_ENTER)) gCurrentScene->setNextScene(1);
        if (IsKeyPressed(KEY_ONE))    gCurrentScene->setNextScene(3);
        if (IsKeyPressed(KEY_TWO))    gCurrentScene->setNextScene(4);
        if (IsKeyPressed(KEY_THREE))  gCurrentScene->setNextScene(5);
        if (IsKeyPressed(KEY_L))     gCurrentScene->setNextScene(8);
        if (IsKeyPressed(KEY_W))     gCurrentScene->setNextScene(7);
    }
    else if (gCurrentScene == gControlsScreen) {
        if (IsKeyPressed(KEY_ENTER)) gCurrentScene->setNextScene(2);
    }
    else if (gCurrentScene == gInstructionsLevelA) {
        if (IsKeyPressed(KEY_ENTER)) gCurrentScene->setNextScene(3);
    }
    else if (gCurrentScene == gLevelCTransition) {
        if (IsKeyPressed(KEY_ENTER)) gCurrentScene->setNextScene(6);
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

        if (IsKeyPressed(KEY_SPACE))
        {
            if (gCurrentScene == gLevelA && gLevelA != nullptr) gLevelA->exitCar();
            else if (gCurrentScene == gLevelB && gLevelB != nullptr) gLevelB->exitCar();
            else if (gCurrentScene == gLevelC && gLevelC != nullptr) gLevelC->exitCar();
        }
    }
    else
    {
        if      (IsKeyDown(KEY_A)) state.witch->moveLeft();
        else if (IsKeyDown(KEY_D)) state.witch->moveRight();

        if      (IsKeyDown(KEY_W)) state.witch->moveUp();
        else if (IsKeyDown(KEY_S)) state.witch->moveDown();

        if (IsKeyPressed(KEY_J))
        {
            state.witch->attack();
            PlaySound(gCurrentScene->getState().witchAttack);
        }

        if (IsKeyPressed(KEY_F) && gCurrentScene == gLevelC) {
            flashlightOn = !flashlightOn;
        }
    }

    if (GetLength(controlledEntity->getMovement()) > 1.0f) 
        controlledEntity->normaliseMovement();
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
        
        if (gCurrentScene == gLevelA || gCurrentScene == gLevelB)
        {
            Entity *witchPtr = gCurrentScene->getState().witch;
            if (witchPtr != nullptr)
            {
                Vector2 cameraTarget = witchPtr->getPosition();
                gEffects->update(FIXED_TIMESTEP, &cameraTarget);
            }
        }

        if (gCurrentScene == gLevelC)
        {
            GameState currentState = gCurrentScene->getState();
            bool driving = currentState.drivingCar && currentState.car != nullptr && currentState.car->isActive();

            if (driving)
            {
                gLightPosition = currentState.car->getPosition();
                gLightPosition.y -= 150.0f;
                lightActive = true;
                flashlightOn = true;
            }
            else if (currentState.witch != nullptr && flashlightOn)
            {
                gLightPosition = currentState.witch->getPosition();
                lightActive = true;
            }
            else
            {
                lightActive = false;
            }
        }
        else
        {
            lightActive = false;
        }
     }
}

void render()
{
    GameState state = gCurrentScene->getState();
    if (gCurrentScene == gLevelA || gCurrentScene == gLevelB || gCurrentScene == gLevelC)
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
    
    if (gCurrentScene == gLevelC) {
        gShader.begin();
        gShader.setInt("applyShader", applyShader);
        Vector2 lightPos = lightActive ? gLightPosition : Vector2{-10000.0f, -10000.0f};
        gShader.setVector2("lightPosition", lightPos);
        gCurrentScene->render();
        gShader.end();
    }
    else
    {
        gCurrentScene->render();
    }

    if (gCurrentScene == gLevelA || gCurrentScene == gLevelB || gCurrentScene == gLevelC) {
        for (Entity *heart : gLifeHearts) heart->render();
    }

    if (gCurrentScene == gLevelA || gCurrentScene == gLevelB)
    {
        gEffects->render();
    }

    EndMode2D();
    EndDrawing();
}

void shutdown() 
{
    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;
    gShader.unload();
    gEffects = nullptr;
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
