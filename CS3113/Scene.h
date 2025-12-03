#include "Entity.h"

#ifndef SCENE_H
#define SCENE_H

struct GameState
{
    Map *map;

    Entity *witch;
    Entity *enemy;
    Entity *car;

    Camera2D camera;

    int maxLives = 3;
    int livesRemaining = 3;

    bool drivingCar = false;
    bool carUnlocked = false;

    int nextSceneID = -1;

    Music bgm;
    Sound carStart;
    Sound enemyAttack;
    Sound witchAttack;
    Sound keyCollect;
    Sound barrierOpen;

};

class Scene 
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode = "#000000";
    
public:
    Scene();
    Scene(Vector2 origin, const char *bgHexCode);

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    GameState   getState()           const { return mGameState; }
    void        setNextScene(int id)       { mGameState.nextSceneID = id; }
    void        resetLivesToMax()    { mGameState.livesRemaining = mGameState.maxLives; }
    Vector2     getOrigin()          const { return mOrigin;    }
    const char* getBGColourHexCode() const { return mBGColourHexCode; }

};

#endif
