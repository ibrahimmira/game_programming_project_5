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
   Vector2 playableCarSpawnPos = { mOrigin.x + 500.0f, mOrigin.y + 450.0f };

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.witch = new Entity(
      {mOrigin.x, playableCarSpawnPos.y},
      {100.0f * sizeRatio, 100.0f},
      witchTextures,
      ATLAS,
      witchSpritesheetDimensions,
      witchAnimationAtlas,
      PLAYER
   );

   // mGameState.witch->setJumpingPower(550.0f);
   mGameState.witch->setColliderDimensions({
      mGameState.witch->getScale().x / 3.3f,
      mGameState.witch->getScale().y
   });
   // tAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
   // mGameState.witch->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED / 2);
   mPlayerCurrentHealth     = mPlayerMaxHealth;
   mPlayerCanTakeDamage     = true;
   mPlayerInvincibilityTimer = 0.0f;

   /*
      -----------    CARS     -----------
   */

   const float laneOffsets[] = { -150.0f, -50.0f, 50.0f, 150.0f };
   const float laneSpeeds[]  = { 350.0f, 320.0f, 300.0f, 280.0f };
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
         "assets/car_01_v4.png",
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
      "assets/car_07.png",
      PLAYER
   );

   playableCar->setColliderDimensions({
      playableCar->getScale().x / 1.6f ,
      playableCar->getScale().y / 1.3f
   });
   playableCar->setSpeed(250);
   mGameState.car = playableCar;
   mGameState.drivingCar = false;
   mGameState.carUnlocked = false;
   // mCarUnlocked = false;
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

   /* 
      ----------- ENEMY AI -----------
   */
   std::map<Animation, std::vector<int>> enemyAnimationAtlas = 
   {
      {WALK_LEFT,  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }},
      {WALK_RIGHT, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }},
      {IDLE_LEFT,  { 1, 2, 3, 4, 5, 6 }      },
      {IDLE_RIGHT, { 1, 2, 3, 4, 5, 6 }      },
      {ATTACK_1_LEFT,  { 8, 7, 6, 5, 4, 3, 2, 1 }},
      {ATTACK_1_RIGHT, { 1, 2, 3, 4, 5, 6, 7, 8 }},

   };

   std::map<Animation, std::string> enemyTextures = 
   {
      {WALK_LEFT,  "assets/witch_enemy/Walk_LEFT.png"  },
      {WALK_RIGHT, "assets/witch_enemy/Walk_RIGHT.png" },
      {IDLE_LEFT,  "assets/witch_enemy/Idle_LEFT.png"      },
      {IDLE_RIGHT, "assets/witch_enemy/Idle_RIGHT.png"     },
      {ATTACK_1_LEFT,  "assets/witch_enemy/Attack_1_LEFT.png"  },
      {ATTACK_1_RIGHT, "assets/witch_enemy/Attack_1_RIGHT.png" },
     
   };

   std::map<Animation, Vector2> enemySpritesheetDimensions = 
   {
      {WALK_LEFT,  { 1, 10 }},
      {WALK_RIGHT, { 1, 10 }},
      {IDLE_LEFT,  { 1, 6 }},
      {IDLE_RIGHT, { 1, 6 }},
      {ATTACK_1_LEFT,  { 1, 8 }},
      {ATTACK_1_RIGHT, { 1, 8 }}
   };

   float sizeRatio2  = 123.0f / 182.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.enemy = new Entity(
      {playableCarSpawnPos.x + 300.0f, playableCarSpawnPos.y},
      {250.0f * sizeRatio2, 250.0f},
      enemyTextures,
      ATLAS,
      enemySpritesheetDimensions,
      enemyAnimationAtlas,
      NPC
   );
   mGameState.enemy->setAIType(WANDERER);
   mGameState.enemy->setSpeed(100);
   // mGameState.enemy->setJumpingPower(550.0f);
   mGameState.enemy->setColliderDimensions({
      mGameState.enemy->getScale().x / 3.5f,
      mGameState.enemy->getScale().y / 3.0f
   });
   // mGameState.enemy->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
   // mGameState.enemy->setFrameSpeed(Entity::DEFAULT_FRAME_SPEED / 2);
   mEnemySpawnPoint = {
      mOrigin.x + 200.0f,
      playableCarSpawnPos.y
   };

   mGameState.enemy->setPosition(mEnemySpawnPoint);
   mGameState.enemy->setWanderRange(mEnemySpawnPoint.x, mEnemyPatrolHalfWidth);
   mEnemyCurrentHealth = mEnemyMaxHealth;
   mEnemyAttackTimer = 0.0f;
   mEnemyAggro = false;
   mEnemyCanTakeDamage = true;

    
}

void LevelA::update(float deltaTime)
{

   float leftBoundary  = mGameState.map->getLeftBoundary();
   float rightBoundary = mGameState.map->getRightBoundary();

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

   Entity **collidables = mTrafficCars.empty() ? nullptr : mTrafficCars.data();
   int collidableCount = (int) (mTrafficCars.size());

   mGameState.witch->update(
      deltaTime,      // delta time / fixed timestep
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

   bool enemyActive = mGameState.enemy->isActive();

   if (enemyActive)
   {
      Vector2 enemyPosition = mGameState.enemy->getPosition();
      float playerDistance  = Vector2Distance(enemyPosition, witchPosition);

      if (!mEnemyAggro && mGameState.witch->isActive() && playerDistance <= mEnemyAggroDistance)
      {
         mEnemyAggro = true;
         mEnemyAttackTimer = 0.0f;
      }
      else if (mEnemyAggro && playerDistance >= mEnemyDisengageDistance)
      {
         mEnemyAggro = false;
         mEnemyAttackTimer = 0.0f;
      }

      // else if (mGameState.witch->isActive() == false)
      // {
      //    mEnemyAggro = false;
      //    mEnemyAttackTimer = 0.0f;
      // }

      if (mEnemyAggro)
      {
         mGameState.enemy->setAIType(FOLLOWER);
         mGameState.enemy->setAIState(WALKING);
         mEnemyAttackTimer += deltaTime;

         if (mEnemyAttackTimer >= mEnemyAttackCooldown)
         {
            mGameState.enemy->attack();
            mEnemyAttackTimer = 0.0f;
         }
      }
      else
      {
         mGameState.enemy->setAIType(WANDERER);
      }
   }

   mGameState.enemy->update(
      deltaTime,      // delta time / fixed timestep
      mGameState.witch,        // player
      nullptr, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   if (mGameState.drivingCar && playableCar->isActive())
   {
      playableCar->update(
         deltaTime,
         nullptr,
         nullptr,
         nullptr,
         0
      );
   }

   enemyActive = mGameState.enemy->isActive();

   if (enemyActive)
   {
      bool playerAttacking = mGameState.witch->getAnimation() == ATTACK_1_LEFT ||
                             mGameState.witch->getAnimation() == ATTACK_1_RIGHT;

      if (!playerAttacking)
      {
         mEnemyCanTakeDamage = true;
      }
      else if (mEnemyCanTakeDamage && mGameState.witch->collidesWith(mGameState.enemy))
      {
         mEnemyCurrentHealth--;
         mEnemyCanTakeDamage = false;

         if (mEnemyCurrentHealth <= 0)
         {
            mGameState.enemy->deactivate();
            mEnemyAggro = false;
            mGameState.enemy->setAIType(WANDERER);
            // mCarUnlocked = true;
            mGameState.carUnlocked = true;
         }
      }

      bool enemyAttacking = mGameState.enemy->getAnimation() == ATTACK_1_LEFT ||
                            mGameState.enemy->getAnimation() == ATTACK_1_RIGHT;

      if (enemyAttacking && mPlayerCanTakeDamage &&
          mGameState.enemy->collidesWith(mGameState.witch))
      {
         mPlayerCurrentHealth--;
         if (mPlayerCurrentHealth < 0) mPlayerCurrentHealth = 0;
         mPlayerCanTakeDamage = false;
         mPlayerInvincibilityTimer = mPlayerInvincibilityDuration;

      }
   }

   if (!mPlayerCanTakeDamage)
   {
      mPlayerInvincibilityTimer -= deltaTime;
      if (mPlayerInvincibilityTimer <= 0.0f)
      {
         mPlayerCanTakeDamage = true;
         mPlayerInvincibilityTimer = 0.0f;
      }
   }

   // if (mCarUnlocked && !mGameState.drivingCar && playableCar->isActive() &&
   //     mGameState.witch->isActive() && !mRecentlyExitedCar &&
   //     mGameState.witch->collidesWith(playableCar))
   // {
   //    mGameState.drivingCar = true;
   //    mGameState.witch->deactivate();
   // }

   if (mGameState.carUnlocked && !mGameState.drivingCar && playableCar->isActive() &&
       mGameState.witch->isActive() && !mRecentlyExitedCar &&
       mGameState.witch->collidesWith(playableCar))
   {
      mGameState.drivingCar = true;
      mGameState.witch->deactivate();
   }

   if (mPlayerCurrentHealth <= 0 && mGameState.witch->isActive())
   {
      mGameState.witch->deactivate();
      mEnemyAggro = false;
      mGameState.enemy->setAIType(WANDERER);
   }

   if (mGameState.drivingCar && playableCar->isActive())
   {
      for (Entity *traffic : mTrafficCars)
      {
         if (playableCar->collidesWith(traffic))
         {
            playableCar->deactivate();
            mGameState.drivingCar = false;
            mGameState.carUnlocked = false;
            mGameState.witch->setPosition(playableCar->getPosition());
            mGameState.witch->activate();
            mRecentlyExitedCar = true;
            mCarExitTimer = mCarExitCooldown;
            break;
         }
      }
   }

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

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   mGameState.map->render();
   mGameState.witch->render();
   mGameState.enemy->render();

   for (Entity *traffic : mTrafficCars) traffic->render();

   playableCar->render();

  
}


void LevelA::shutdown()
{
  for (Entity *traffic : mTrafficCars) delete traffic;
  mTrafficCars.clear();
  mTrafficSpeeds.clear();
  delete mGameState.witch;  
  delete mGameState.map;
  delete mGameState.enemy;
  delete playableCar;
}

void LevelA::exitCar()
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
