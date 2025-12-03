#include <vector>
#include "LevelA.h"

constexpr int LEVEL_WIDTH_B = 14,
              LEVEL_HEIGHT_B = 16;

class LevelB : public Scene {
private:
    unsigned int mLevelData[LEVEL_WIDTH_B * LEVEL_HEIGHT_B] = {
        22, 4,   3, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
        17, 34, 35, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 18,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        1, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        1, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        17, 4,  3, 30, 30, 30, 30, 30, 30, 30, 30,  30, 30, 18,
        23, 4,  3, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 24,
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
    // Second enemy (guards red button)
    Entity *mSecondEnemy            = nullptr;
    int     mSecondEnemyMaxHealth   = 3;
    int     mSecondEnemyCurrentHealth = 3;
    bool    mSecondEnemyCanTakeDamage = true;
    float   mSecondEnemyAttackCooldown = 2.0f;
    float   mSecondEnemyAttackTimer    = 0.0f;
    bool    mSecondEnemyAggro          = false;
    bool    mCarKeySpawned          = false;
    bool    mCarKeyCollected        = false;
    float   mCarKeySpawnOffset      = 200.0f;
    int     mPlayerMaxHealth        = 8;
    int     mPlayerCurrentHealth    = 8;
    float   mPlayerInvincibilityDuration = 0.75f;
    float   mPlayerInvincibilityTimer    = 0.0f;
    bool    mPlayerCanTakeDamage    = true;
    bool    mRecentlyExitedCar      = false;
    float   mCarExitCooldown        = 2.25f;
    float   mCarExitTimer           = 0.0f;

    std::vector<Entity*> mTrafficCars;
    std::vector<float>   mTrafficSpeeds;

public:
    static constexpr float  TILE_DIMENSION       = 100.0f,
                            END_GAME_THRESHOLD      = 800.0f,
                            CAR_SPEED              = 300.0f;

    Entity* playableCar = nullptr;
    Entity* barrier     = nullptr;
    Entity* carKey      = nullptr;
    Entity* redButton    = nullptr;
    LevelB();
    LevelB(Vector2 origin, const char *bgHexCode);
    ~LevelB();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void exitCar();
};
