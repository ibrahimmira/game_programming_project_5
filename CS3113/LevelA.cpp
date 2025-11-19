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
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/Racetrack.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      7, 8,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */

   // std::map<Direction, std::vector<int>> AnimationAtlas = {
   //    { DOWN,     {  9,  10, 11, 12, 13, 14, 15, 16, 17 } },
   //    { UP,       {  27, 28, 29, 30, 31, 32, 33, 34, 35 } },
   //    { RIGHT,    {  0,  1,  2,  3,  4,  5, 6, 7, 8} },
   //    { LEFT,     {  18, 19, 20, 21, 22, 23, 24, 25, 26 } }
   // };
   
   // float sizeRatio  = 36.0f / 55.0f;

   // mGameState.hero = new Entity(
   //    {mOrigin.x, mOrigin.y}, // position
   //    {85.0f * sizeRatio, 85.0f},             // scale
   //    "assets/hero.png",                   // texture file address
   //    ATLAS,                                    // single image or atlas?
   //    { 4, 9 },                                 // atlas dimensions
   //    AnimationAtlas,                    // actual atlas
   //    PLAYER                                    // entity type
   // );

   // mGameState.hero->setJumpingPower(550.0f);
   // mGameState.hero->setColliderDimensions({
   //    mGameState.hero->getScale().x / 1.5f,
   //    mGameState.hero->getScale().y / 1.2f
   // });

   // mGameState.hero->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   std::map<Animation, std::vector<int>> witchAnimationAtlas = 
   {
      {WALK_LEFT,  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }},
      {WALK_RIGHT, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }},
      {IDLE_LEFT,  { 1, 2, 3, 4, 5, 6 }      },
      {IDLE_RIGHT, { 1, 2, 3, 4, 5, 6 }      },
      {ATTACK_1_LEFT,  { 4, 3, 2, 1, 0}},
      {ATTACK_1_RIGHT, { 1, 2, 3, 4, 5}},

   };

   std::map<Animation, std::string> witchTextures = 
   {
      {WALK_LEFT,  "assets/witch/Walk_LEFT.png"  },
      {WALK_RIGHT, "assets/witch/WAlk_RIGHT.png" },
      {IDLE_LEFT,  "assets/witch/Idle_LEFT.png"      },
      {IDLE_RIGHT, "assets/witch/Idle_RIGHT.png"     },
      {ATTACK_1_LEFT,  "assets/witch/Attack_1_LEFT.png"  },
      {ATTACK_1_RIGHT, "assets/witch/Attack_1_RIGHT.png" },
     
   };

   std::map<Animation, Vector2> witchSpritesheetDimensions = 
   {
      {WALK_LEFT,  { 1, 10 }},
      {WALK_RIGHT, { 1, 10 }},
      {IDLE_LEFT,  { 1, 6 }},
      {IDLE_RIGHT, { 1, 6 }},
      {ATTACK_1_LEFT,  { 1, 5 }},
      {ATTACK_1_RIGHT, { 1, 5 }}
   };

   float sizeRatio  = 122.0f / 70.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.witch = new Entity(
      {mOrigin.x, mOrigin.y - 200.0f},
      {100.0f * sizeRatio, 100.0f},
      witchTextures,
      ATLAS,
      witchSpritesheetDimensions,
      witchAnimationAtlas,
      PLAYER
   );

   mGameState.witch->setJumpingPower(550.0f);
   mGameState.witch->setColliderDimensions({
      mGameState.witch->getScale().x / 3.3f,
      mGameState.witch->getScale().y
   });
   // tAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
   mGameState.witch->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED / 2);

   /*
      -----------    CARS     -----------
   */

   cars = new Entity(
      {mOrigin.x, mOrigin.y - 350},
      { 530 / 3, 258 / 3 },
      "assets/car_01_v4.png",
      BLOCK
   );

   cars->setColliderDimensions({
      cars->getScale().x ,
      cars->getScale().y
   });

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.witch->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom

   /* 
      ----------- ENEMY AI -----------
   */


    
}

void LevelA::update(float deltaTime)
{

   float halfWidth     = cars->getScale().x / 2.0f;
   float resetPosition = mGameState.map->getLeftBoundary() - 4 * halfWidth;
   float wrapPoint     = mGameState.map->getRightBoundary() + 4 * halfWidth;

   Vector2 position = cars->getPosition();
   position.x += CAR_SPEED * deltaTime;

   if (position.x > wrapPoint)
   {
      position.x = resetPosition;
   }

   cars->setPosition(position);

   mGameState.witch->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      nullptr, // map
      cars,        // collidable entities
      1               // col. entity count
   );
   Vector2 currentPlayerPosition = mGameState.witch->getPosition();
   panCamera(&mGameState.camera, &currentPlayerPosition);
  
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   mGameState.map->render();
   mGameState.witch->render();
   cars->render();

  
}


void LevelA::shutdown()
{
  delete cars;
  delete mGameState.witch;  
  delete mGameState.map;
}
