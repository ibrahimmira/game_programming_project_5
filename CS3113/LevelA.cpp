#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.nextSceneID = -1;

   /*
      ----------- MAP -----------
   */

   /*
      ----------- PROTAGONIST -----------
   */


   /*
      ----------- CAMERA -----------
   */


   /* 
      ----------- ENEMY AI -----------
   */

    
}

void LevelA::update(float deltaTime)
{
   // Update game objects
  
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

  
}


void LevelA::shutdown()
{
  
}