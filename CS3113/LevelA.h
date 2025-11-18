#include "Menu.h"

constexpr int LEVEL_WIDTH = 16,
              LEVEL_HEIGHT = 9;

class LevelA : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {};

public:
    static constexpr float TILE_DIMENSION       = 70.0f,
                        ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f;

    Entity* gold = nullptr;
    LevelA();
    LevelA(Vector2 origin, const char *bgHexCode);
    ~LevelA();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};