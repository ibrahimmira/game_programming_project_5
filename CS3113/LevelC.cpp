#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f}, nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{

   mGameState.bgm = LoadMusicStream("assets/music_and_sounds/levelA_looping.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);
   mGameState.carStart     = LoadSound("assets/music_and_sounds/car_startup.mp3");
   mGameState.keyCollect   = LoadSound("assets/music_and_sounds/key_collect.mp3");
   mGameState.witchAttack  = LoadSound("assets/music_and_sounds/witch_hit.mp3");
   mGameState.barrierOpen  = LoadSound("assets/music_and_sounds/barrier_open.mp3");
   SetSoundVolume(mGameState.keyCollect, 10.25f);


   mGameState.nextSceneID = -1;

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH_C, LEVEL_HEIGHT_C,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/Racetrack.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      7, 8,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   
   /*
      -----------  PLAYER  -----------
   */
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
      {WALK_LEFT,  "assets/witch_protag/Walk_LEFT.png"  },
      {WALK_RIGHT, "assets/witch_protag/WAlk_RIGHT.png" },
      {IDLE_LEFT,  "assets/witch_protag/Idle_LEFT.png"      },
      {IDLE_RIGHT, "assets/witch_protag/Idle_RIGHT.png"     },
      {ATTACK_1_LEFT,  "assets/witch_protag/Attack_1_LEFT.png"  },
      {ATTACK_1_RIGHT, "assets/witch_protag/Attack_1_RIGHT.png" },
     
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
   Vector2 playableCarSpawnPos = { mOrigin.x, mGameState.map->getBottomBoundary() -120.0f };

   mGameState.witch = new Entity(
      {mOrigin.x, playableCarSpawnPos.y - 200.0f},
      {100.0f * sizeRatio, 100.0f},
      witchTextures,
      ATLAS,
      witchSpritesheetDimensions,
      witchAnimationAtlas,
      PLAYER
   );

   mGameState.witch->setSpeed(150);

   mGameState.witch->setColliderDimensions({
      mGameState.witch->getScale().x / 3.3f,
      mGameState.witch->getScale().y
   });
   
   mPlayerCurrentHealth     = mPlayerMaxHealth;
   mPlayerCanTakeDamage     = true;
   mPlayerInvincibilityTimer = 0.0f;

   /*
      -----------    CARS     -----------
   */

   const float laneOffsets[] = {
      -1600.0f, -1500.0f, -1400.0f, -1300.0f,
      -1000.0f, -900.0f,
      -600.0f,  -500.0f,
      -200.0f,  -100.0f,
      200.0f,    300.0f,
      600.0f,    700.0f,
      1000.0f,   1100.0f
   };

   // More difficult lane speeds
   // const float laneSpeeds[]  = { -300.0f, -400.0f, -300.0f, -500.0f,
   //                               -700.0f, -600.0f,
   //                               -600.0f, -700.0f,
   //                               600.0f, 800.0f, 
   //                               800.0f, 700.0f,
   //                               900.0f, 500.0f,
   //                               300.0f, 400.0f };

   const float laneSpeeds[] = {
      300.0f, 400.0f, 300.0f, 400.0f,
      300.0f, 400.0f,
      300.0f, 400.0f,
      300.0f, 400.0f,
      300.0f, 400.0f,
      300.0f, 400.0f,
      300.0f, 400.0f
   };

   const int laneCount = sizeof(laneOffsets) / sizeof(float);

   for (int i = 0; i < laneCount; i++)
   {
      Vector2 spawn = {
         (laneSpeeds[i] > 0 ? mGameState.map->getLeftBoundary() - 300.0f
                            : mGameState.map->getRightBoundary() + 300.0f),
         mOrigin.y + laneOffsets[i]
      };

      Entity *trafficCar = new Entity(
         spawn,
         { 530 / 3, 258 / 3 },
         "assets/car_28_v3.png",
         NPC
      );

      trafficCar->setColliderDimensions({
         trafficCar->getScale().x,
         trafficCar->getScale().y
      });

      mTrafficCars.push_back(trafficCar);
      mTrafficSpeeds.push_back(laneSpeeds[i]);
   }

   playableCar = new Entity(
      playableCarSpawnPos,
      { 440 / 3, 680 / 3 },
      "assets/car_34.png",
      PLAYER
   );

   playableCar->setColliderDimensions({
      playableCar->getScale().x / 1.6f ,
      playableCar->getScale().y / 1.3f
   });

   playableCar->setSpeed(200);
   mGameState.car = playableCar;
   mGameState.drivingCar = false;
   mGameState.carUnlocked = false;
   mRecentlyExitedCar = false;
   mCarExitTimer = 0.0f;

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.witch->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom

   mBarriers.clear();
   mBarrierSpeeds.clear();
   mBarrierMinX.clear();
   mBarrierMaxX.clear();

   float laneStartY = mGameState.map->getTopBoundary() + 200.0f;
   float laneSpacing = 260.0f;
   float barrierWidth = 1280.0f / 12.0f;
   float barrierHeight = 366.0f / 8.0f;

   for (int i = 0; i < 3; i++)
   {
      bool fromLeft = (i % 2 == 0);
      float startX = fromLeft ? mGameState.map->getLeftBoundary() + 650.0f
                              : mGameState.map->getRightBoundary() - 650.0f;
      float laneY = laneStartY + i * laneSpacing;

      Entity *moving = new Entity(
         { startX, laneY },
         { barrierWidth, barrierHeight },
         "assets/barrier.png",
         NPC
      );
      moving->setColliderDimensions({ barrierWidth, barrierHeight });

      float travel = 400.0f;
      float minX = fromLeft ? startX : startX - travel;
      float maxX = fromLeft ? startX + travel : startX;
      float speed = fromLeft ? 120.0f : -120.0f;

      mBarriers.push_back(moving);
      mBarrierSpeeds.push_back(speed);
      mBarrierMinX.push_back(minX);
      mBarrierMaxX.push_back(maxX);
   }

   /* 
      ----------- Car Key -----------
   */
   Rectangle keyZones[2] = {
      { 26.0f, 1623.0f, 536.0f - 26.0f, 2440.0f - 1623.0f },
      { 834.0f, 1623.0f, 1370.0f - 834.0f, 2440.0f - 1623.0f }
   };
   int zoneIndex = GetRandomValue(0, 1);
   Rectangle zone = keyZones[zoneIndex];
   float randX = (float) GetRandomValue((int) zone.x, (int) (zone.x + zone.width));
   float randY = (float) GetRandomValue((int) zone.y, (int) (zone.y + zone.height));
   Vector2 carKeyPosition = {
      randX,
      randY
   };

   carKey = new Entity(
      carKeyPosition,
      { 80.0f, 80.0f },
      "assets/car_key.png",
      BLOCK
   );
   carKey->setColliderDimensions({
      carKey->getScale().x * 0.6f,
      carKey->getScale().y * 0.6f
   });
   mCarKeySpawned = true;
   mCarKeyCollected = false;

    
}

void LevelC::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   // Update traffic cars
   float leftBoundary  = mGameState.map->getLeftBoundary();
   float rightBoundary = mGameState.map->getRightBoundary();
   float topBoundary   = mGameState.map->getTopBoundary();
   float bottomBoundary = mGameState.map->getBottomBoundary();

   for (size_t i = 0; i < mTrafficCars.size(); i++)
   {
      Entity *traffic = mTrafficCars[i];
      float speed = mTrafficSpeeds[i];

      Vector2 position = traffic->getPosition();
      position.x += speed * deltaTime;

      float halfWidth = traffic->getScale().x / 2.0f;
      float leftWrap  = leftBoundary  - 4.0f * halfWidth;
      float rightWrap = rightBoundary + 4.0f * halfWidth;

      if (speed > 0.0f && position.x > rightWrap) position.x = leftWrap;
      else if (speed < 0.0f && position.x < leftWrap) position.x = rightWrap;

      traffic->setPosition(position);
   }

   std::vector<Entity*> collidableEntities = mTrafficCars;
   
   Entity **collidables = collidableEntities.empty() ? nullptr : collidableEntities.data();
   int collidableCount = (int) (collidableEntities.size());

   Entity *carCollidables[1];
   int carCollidableCount = 0;

   // Update witch (player)
   mGameState.witch->update(
      deltaTime,      // delta time
      nullptr,        // player
      nullptr,        // map
      collidables,    // collidable entities
      collidableCount               // col. entity count
   );

   Vector2 witchPosition = mGameState.witch->getPosition();
   Vector2 witchColliderSize = mGameState.witch->getColliderDimensions();

   float halfColliderWidth  = witchColliderSize.x / 2.0f;
   float halfColliderHeight = witchColliderSize.y / 2.0f;

   float minPlayerXpos = mGameState.map->getLeftBoundary()   + halfColliderWidth;
   float maxPlayerXpos = mGameState.map->getRightBoundary()  - halfColliderWidth;
   float minPlayerYpos = mGameState.map->getTopBoundary()    + halfColliderHeight;
   float maxPlayerYpos = mGameState.map->getBottomBoundary() - halfColliderHeight;

   bool clamped = false;

   if (witchPosition.x < minPlayerXpos) { witchPosition.x = minPlayerXpos; clamped = true; }
   else if (witchPosition.x > maxPlayerXpos) { witchPosition.x = maxPlayerXpos; clamped = true; }

   if (witchPosition.y < minPlayerYpos) { witchPosition.y = minPlayerYpos; clamped = true; }
   else if (witchPosition.y > maxPlayerYpos) { witchPosition.y = maxPlayerYpos; clamped = true; }

   if (clamped) mGameState.witch->setPosition(witchPosition);

   if (mGameState.drivingCar && playableCar->isActive())
   {
      Vector2 carPosition = playableCar->getPosition();
      Vector2 carCollider = playableCar->getColliderDimensions();

      float carHalfWidth  = carCollider.x / 2.0f;
      float carHalfHeight = carCollider.y / 2.0f;

      float minCarXpos = leftBoundary   + carHalfWidth;
      float maxCarXpos = rightBoundary  - carHalfWidth;
      float maxCarYpos = mGameState.map->getBottomBoundary() - carHalfHeight;

      if (carPosition.x < minCarXpos)      carPosition.x = minCarXpos;
      else if (carPosition.x > maxCarXpos) carPosition.x = maxCarXpos;

      if (carPosition.y > maxCarYpos) carPosition.y = maxCarYpos;

      playableCar->setPosition(carPosition);

      if (carPosition.y + carHalfHeight < mGameState.map->getTopBoundary())
      {
         mGameState.nextSceneID = 7;
      }
   }

   if (mRecentlyExitedCar)
   {
      mCarExitTimer -= deltaTime;

      if (mCarExitTimer <= 0.0f)
      {
         mRecentlyExitedCar = false;
         mCarExitTimer = 0.0f;
      }
   }

   // Update playable car
   if (mGameState.drivingCar && playableCar->isActive())
   {
      playableCar->update(
         deltaTime,
         nullptr,
         nullptr,
         (carCollidableCount > 0 ? carCollidables : nullptr),
         carCollidableCount
      );
   }

   for (size_t i = 0; i < mBarriers.size(); i++)
   {
      Entity *moving = mBarriers[i];
      float speed = mBarrierSpeeds[i];
      Vector2 pos = moving->getPosition();
      pos.x += speed * deltaTime;

      if (pos.x < mBarrierMinX[i] || pos.x > mBarrierMaxX[i])
      {
         mBarrierSpeeds[i] = -mBarrierSpeeds[i];
         pos.x = fmaxf(mBarrierMinX[i], fminf(pos.x, mBarrierMaxX[i]));
      }

      moving->setPosition(pos);
   }

   if (mCarKeySpawned && !mCarKeyCollected && carKey != nullptr &&
       carKey->isActive() && mGameState.witch->isActive() &&
       mGameState.witch->collidesWith(carKey))
   {
      mCarKeyCollected = true;
      PlaySound(mGameState.keyCollect);
      mGameState.carUnlocked = true;
      carKey->deactivate();
   }

   if (mGameState.carUnlocked && !mGameState.drivingCar && playableCar->isActive() &&
       mGameState.witch->isActive() && !mRecentlyExitedCar &&
       mGameState.witch->collidesWith(playableCar))
   {
      mGameState.drivingCar = true;
      PlaySound(mGameState.carStart);
      mGameState.witch->deactivate();
   }

   if (mPlayerCurrentHealth <= 0 && mGameState.witch->isActive())
   {
      mGameState.witch->deactivate();

      if (mGameState.livesRemaining > 0) mGameState.livesRemaining--;

      mGameState.nextSceneID = (mGameState.livesRemaining <= 0) ? 8 : 6; // lose or respawn level 
   }

  if (mGameState.drivingCar && playableCar->isActive())
  {
     for (size_t i = 0; i < mTrafficCars.size(); i++)
     {
        Entity *traffic = mTrafficCars[i];
        if (playableCar->collidesWith(traffic))
        {
           if (mGameState.livesRemaining > 0) mGameState.livesRemaining--;
           mGameState.nextSceneID = (mGameState.livesRemaining <= 0) ? 8 : 6;

           playableCar->deactivate();
           mGameState.drivingCar = false;
           mGameState.carUnlocked = false;
            if (mGameState.witch != nullptr)
            {
               mGameState.witch->setPosition(playableCar->getPosition());
               mGameState.witch->activate();
            }
            mRecentlyExitedCar = true;
            mCarExitTimer = mCarExitCooldown;
            return;
        }
     }

     for (size_t i = 0; i < mBarriers.size(); i++)
     {
        Entity *moving = mBarriers[i];
        if (moving != nullptr && playableCar != nullptr && playableCar->isActive() && playableCar->collidesWith(moving))
        {
           if (mGameState.livesRemaining > 0) mGameState.livesRemaining--;
           mGameState.nextSceneID = (mGameState.livesRemaining <= 0) ? 8 : 6;

           playableCar->deactivate();
           mGameState.drivingCar = false;
           mGameState.carUnlocked = false;
            if (mGameState.witch != nullptr)
            {
               mGameState.witch->setPosition(playableCar->getPosition());
               mGameState.witch->activate();
            }
            mRecentlyExitedCar = true;
            mCarExitTimer = mCarExitCooldown;
            return;
        }
     }
   }

   // Update camera
   Vector2 cameraFocus = (mGameState.drivingCar && playableCar->isActive())
      ? playableCar->getPosition()
      : witchPosition;

   panCamera(&mGameState.camera, &cameraFocus);

   float halfViewHeight = mGameState.camera.offset.y;
   float minCameraYpos = mGameState.map->getTopBoundary() + halfViewHeight;
   float maxCameraYpos = mGameState.map->getBottomBoundary() - halfViewHeight;

   if (minCameraYpos > maxCameraYpos)
   {
      float centerPos = (minCameraYpos + maxCameraYpos) / 2.0f;
      mGameState.camera.target.y = centerPos;
   }
   else
   {
      mGameState.camera.target.y = fmaxf(minCameraYpos, fminf(mGameState.camera.target.y, maxCameraYpos));
   }
   
}

void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   mGameState.map->render();
   mGameState.witch->render();

   if (carKey != nullptr && carKey->isActive()) carKey->render();

   for (Entity *traffic : mTrafficCars) traffic->render();

   playableCar->render();

   for (Entity *moving : mBarriers) moving->render();
}


void LevelC::shutdown()
{
  for (Entity *traffic : mTrafficCars) delete traffic;
  mTrafficCars.clear();
  mTrafficSpeeds.clear();
  delete mGameState.witch;  
  delete mGameState.map;

  delete playableCar;
  for (Entity *moving : mBarriers) delete moving;
  mBarriers.clear();
  delete carKey;

  UnloadMusicStream(mGameState.bgm);
  UnloadSound(mGameState.carStart);
  UnloadSound(mGameState.witchAttack);
  UnloadSound(mGameState.keyCollect);
}

void LevelC::exitCar()
{
   if (!mGameState.drivingCar || playableCar == nullptr || !playableCar->isActive()) return;

   mGameState.drivingCar = false;

   Vector2 carPosition = playableCar->getPosition();
   Vector2 witchCollider = mGameState.witch->getColliderDimensions();

   Vector2 exitPosition = {
      carPosition.x,
      carPosition.y + witchCollider.y
   };

   float halfWidth  = witchCollider.x / 2.0f;
   float halfHeight = witchCollider.y / 2.0f;

   float minXpos = mGameState.map->getLeftBoundary()   + halfWidth;
   float maxXpos = mGameState.map->getRightBoundary()  - halfWidth;
   float minYpos = mGameState.map->getTopBoundary()    + halfHeight;
   float maxYpos = mGameState.map->getBottomBoundary() - halfHeight;

   exitPosition.x = fmaxf(minXpos, fminf(exitPosition.x - 150.f, maxXpos));
   exitPosition.y = fmaxf(minYpos, fminf(exitPosition.y - 150.f, maxYpos));

   mGameState.witch->setPosition(exitPosition);
   mGameState.witch->activate();

   mRecentlyExitedCar = true;
   mCarExitTimer = mCarExitCooldown;
}
