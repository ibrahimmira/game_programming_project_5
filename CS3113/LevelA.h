#include <vector>
#include "Menu.h"

constexpr int LEVEL_WIDTH = 14,
              LEVEL_HEIGHT = 12;

class LevelA : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        22, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 4,  3, 21,
        17, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 34, 35, 18,
        17, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 4,  3, 18,
        17, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 4,  3, 18,
        17, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 4,  3, 18,
        17, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 4,  3, 18,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 8, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8, 8, 2,
        17, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 4,  3, 18,
        17, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 4,  3, 18,
        17, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 4,  3, 18,
        23, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 4,  3, 24,
    };

    Vector2 mEnemySpawnPoint        = { 0.0f, 0.0f };
    float   mEnemyPatrolHalfWidth   = 150.0f;
    float   mEnemyAggroDistance     = 250.0f;
    float   mEnemyDisengageDistance = 375.0f;
    float   mEnemyAttackCooldown    = 2.0f;
    float   mEnemyAttackTimer       = 0.0f;
    int     mEnemyMaxHealth         = 3;
    int     mEnemyCurrentHealth     = 3;
    bool    mEnemyAggro             = false;
    bool    mEnemyCanTakeDamage     = true;
    bool    mCarKeySpawned          = false;
    bool    mCarKeyCollected        = false;
    float   mCarKeySpawnOffset      = 200.0f;
    int     mPlayerMaxHealth        = 8;
    int     mPlayerCurrentHealth    = 8;
    float   mPlayerInvincibilityDuration = 0.75f;
    float   mPlayerInvincibilityTimer    = 0.0f;
    bool    mPlayerCanTakeDamage    = true;
    //bool    mCarUnlocked            = false;
    bool    mRecentlyExitedCar      = false;
    float   mCarExitCooldown        = 2.25f;
    float   mCarExitTimer           = 0.0f;

    std::vector<Entity*> mTrafficCars;
    std::vector<float>   mTrafficSpeeds;

public:
    static constexpr float TILE_DIMENSION       = 100.0f,
                        // ACCELERATION_OF_GRAVITY = 981.0f,
                        END_GAME_THRESHOLD      = 800.0f,
                        CAR_SPEED              = 300.0f;

    Entity* playableCar = nullptr;
    Entity* barrier     = nullptr;
    Entity* carKey      = nullptr;
    Entity* redButton    = nullptr;
    LevelA();
    LevelA(Vector2 origin, const char *bgHexCode);
    ~LevelA();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void exitCar();
};
