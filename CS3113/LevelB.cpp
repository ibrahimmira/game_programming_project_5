#include "LevelB.h"

LevelB::LevelB()                                      : Scene { {0.0f}, nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{

   mGameState.bgm = LoadMusicStream("assets/music_and_sounds/Galactic Rap.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.carStart     = LoadSound("assets/music_and_sounds/car_startup.mp3");
   mGameState.enemyAttack  = LoadSound("assets/music_and_sounds/enemy_hit.mp3");
   mGameState.witchAttack  = LoadSound("assets/music_and_sounds/witch_hit.mp3");
   mGameState.keyCollect   = LoadSound("assets/music_and_sounds/key_collect.mp3");
   mGameState.barrierOpen  = LoadSound("assets/music_and_sounds/barrier_open.mp3");
   SetSoundVolume(mGameState.keyCollect, 10.25f);


   mGameState.nextSceneID = -1;

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH_B, LEVEL_HEIGHT_B,   // map grid cols & rows
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
   Vector2 playableCarSpawnPos = { mOrigin.x - 500.0f, mOrigin.y + 650.0f };

   mGameState.witch = new Entity(
      {mOrigin.x, playableCarSpawnPos.y},
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

   const float laneOffsets[] = { -50.0f, 50.0f, 350.0f, 450.0f };
   const float laneSpeeds[]  = { -470.0f, -500.0f, -480.0f, -420.0f };
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
         "assets/car_30_v2.png",
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
      "assets/car_33_v4.png",
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

   mGameState.enemy = new Entity(
      {playableCarSpawnPos.x + 300.0f, playableCarSpawnPos.y},
      {250.0f * sizeRatio2, 250.0f},
      enemyTextures,
      ATLAS,
      enemySpritesheetDimensions,
      enemyAnimationAtlas,
      NPC
   );

   mGameState.enemy->setSpeed(150);

   mGameState.enemy->setAIType(WANDERER);
   mGameState.enemy->setSpeed(100);
   mGameState.enemy->setColliderDimensions({
      mGameState.enemy->getScale().x / 3.5f,
      mGameState.enemy->getScale().y / 3.0f
   });
 
   mEnemySpawnPoint = {
      mOrigin.x + 510.0f,
      playableCarSpawnPos.y
   };

   mGameState.enemy->setPosition(mEnemySpawnPoint);
   mGameState.enemy->setWanderRange(mEnemySpawnPoint.x, mEnemyPatrolHalfWidth);
   mEnemyCurrentHealth = mEnemyMaxHealth;
   mEnemyAttackTimer = 0.0f;
   mEnemyAggro = false;
   mEnemyCanTakeDamage = true;

   // Secondary enemy guarding the red button further up the map
   mSecondEnemy = new Entity(
      { mOrigin.x + 100.0f, mOrigin.y - 200.0f },
      {250.0f * sizeRatio2, 250.0f},
      enemyTextures,
      ATLAS,
      enemySpritesheetDimensions,
      enemyAnimationAtlas,
      NPC
   );

   mSecondEnemy->setSpeed(150);

   mSecondEnemy->setAIType(WANDERER);
   mSecondEnemy->setSpeed(100);
   mSecondEnemy->setColliderDimensions({
      mSecondEnemy->getScale().x / 3.5f,
      mSecondEnemy->getScale().y / 3.0f
   });
   mSecondEnemy->setWanderRange(mSecondEnemy->getPosition().x, mEnemyPatrolHalfWidth);
   mSecondEnemyMaxHealth = 3;
   mSecondEnemyCurrentHealth = mSecondEnemyMaxHealth;
   mSecondEnemyCanTakeDamage = true;
   mSecondEnemyAggro = false;
   mSecondEnemyAttackTimer = 0.0f;

   /* 
      ----------- Barrier -----------
   */
  
   Vector2 barrierPosition = {
      mGameState.map->getLeftBoundary() + 200.0f,
      mGameState.map->getTopBoundary() + 335.0f
   };
   barrier = new Entity(
      barrierPosition,
      { 1280 / 6.8, 366.0f / 8 },
      "assets/barrier.png",
      NPC
   );
   barrier->setColliderDimensions({
      barrier->getScale().x,
      barrier->getScale().y
   });

   /* 
      ----------- Car Key -----------
   */
   carKey = nullptr;
   mCarKeySpawned = false;
   mCarKeyCollected = false;

   /* 
      ----------- Red Button -----------
   */
   Vector2 redButtonPosition = {
      mOrigin.x + 400.0f,
      mOrigin.y - 300.0f
   };
   redButton = new Entity(
      redButtonPosition,
      { 25, 25 },
      "assets/red_button.png",
      NPC
   );
   redButton->setColliderDimensions({
      redButton->getScale().x / 1.1f,
      redButton->getScale().y / 1.2f
   });
   redButton->deactivate(); // only appears after defeating second enemy

    
}

void LevelB::update(float deltaTime)
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
   if (barrier) collidableEntities.push_back(barrier);
   
   Entity **collidables = collidableEntities.empty() ? nullptr : collidableEntities.data();
   int collidableCount = (int) (collidableEntities.size());

   Entity *carCollidables[1];
   int carCollidableCount = 0;
   if (barrier != nullptr)
   {
      carCollidables[carCollidableCount++] = barrier;
   }

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
         mGameState.nextSceneID = 5;
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

   // Update enemy 
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

      if (mEnemyAggro)
      {
         mGameState.enemy->setAIType(FOLLOWER);
         mGameState.enemy->setAIState(WALKING);
         mEnemyAttackTimer += deltaTime;

         if (mEnemyAttackTimer >= mEnemyAttackCooldown)
         {
            mGameState.enemy->attack();
            PlaySound(mGameState.enemyAttack);
            mEnemyAttackTimer = 0.0f;
         }
      }
      else
      {
         mGameState.enemy->setAIType(WANDERER);
      }
   }

   mGameState.enemy->update(
      deltaTime,      // delta time
      mGameState.witch,        // player
      nullptr, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   if (mSecondEnemy != nullptr && mSecondEnemy->isActive())
   {
      float secondEnemyDistance = Vector2Distance(mSecondEnemy->getPosition(), witchPosition);

      if (secondEnemyDistance <= mEnemyAggroDistance)
      {
         mSecondEnemy->setAIType(FOLLOWER);
         mSecondEnemy->setAIState(WALKING);
         mSecondEnemyAggro = true;
      }
      else if (secondEnemyDistance >= mEnemyDisengageDistance)
      {
         mSecondEnemy->setAIType(WANDERER);
         mSecondEnemyAggro = false;
      }

      mSecondEnemy->update(
         deltaTime,
         mGameState.witch,
         nullptr,
         nullptr,
         0
      );

      if (mSecondEnemyAggro)
      {
         mSecondEnemyAttackTimer += deltaTime;
         if (mSecondEnemyAttackTimer >= mSecondEnemyAttackCooldown)
         {
            mSecondEnemy->attack();
            PlaySound(mGameState.enemyAttack);
            mSecondEnemyAttackTimer = 0.0f;
         }
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

            if (!mCarKeySpawned)
            {
               Vector2 carKeyScale = { 80.0f, 80.0f };
               Vector2 carKeyPosition = mGameState.witch->getPosition();
               carKeyPosition.x += mCarKeySpawnOffset;
               carKeyPosition.y -= 40.0f;

               float halfKeyWidth  = carKeyScale.x / 2.0f;
               float halfKeyHeight = carKeyScale.y / 2.0f;

               carKeyPosition.x = fmaxf(leftBoundary + halfKeyWidth,
                                        fminf(carKeyPosition.x, rightBoundary - halfKeyWidth));
               carKeyPosition.y = fmaxf(topBoundary + halfKeyHeight,
                                        fminf(carKeyPosition.y, bottomBoundary - halfKeyHeight));

               carKey = new Entity(
                  carKeyPosition,
                  carKeyScale,
                  "assets/car_key.png",
                  BLOCK
               );

               carKey->setColliderDimensions({
                  carKeyScale.x * 0.6f,
                  carKeyScale.y * 0.6f
               });

               mCarKeySpawned = true;
            }
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

   if (mSecondEnemy != nullptr && mSecondEnemy->isActive())
   {
      bool playerAttacking2 = mGameState.witch->getAnimation() == ATTACK_1_LEFT ||
                              mGameState.witch->getAnimation() == ATTACK_1_RIGHT;

      if (!playerAttacking2)
      {
         mSecondEnemyCanTakeDamage = true;
      }
      else if (mSecondEnemyCanTakeDamage && mGameState.witch->collidesWith(mSecondEnemy))
      {
         mSecondEnemyCurrentHealth--;
         mSecondEnemyCanTakeDamage = false;

         if (mSecondEnemyCurrentHealth <= 0)
         {
            mSecondEnemy->deactivate();
            if (redButton != nullptr) redButton->activate();
         }
      }

      bool secondEnemyAttacking = mSecondEnemy->getAnimation() == ATTACK_1_LEFT ||
                                  mSecondEnemy->getAnimation() == ATTACK_1_RIGHT;

      if (secondEnemyAttacking && mPlayerCanTakeDamage &&
          mSecondEnemy->collidesWith(mGameState.witch))
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

   if (redButton != nullptr && redButton->isActive() &&
       mGameState.witch->isActive() &&
       mGameState.witch->collidesWith(redButton))
   {
      redButton->deactivate();
      if (barrier != nullptr) barrier->deactivate();
      PlaySound(mGameState.barrierOpen);
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
      mEnemyAggro = false;
      mGameState.enemy->setAIType(WANDERER);

      if (mGameState.livesRemaining > 0) mGameState.livesRemaining--;

      mGameState.nextSceneID = (mGameState.livesRemaining <= 0) ? 8 : 4; // lose or respawn level
   }

   if (mGameState.drivingCar && playableCar->isActive())
   {
      for (Entity *traffic : mTrafficCars)
      {
         if (playableCar->collidesWith(traffic))
         {
            if (mGameState.livesRemaining > 0) mGameState.livesRemaining--;
            mGameState.nextSceneID = (mGameState.livesRemaining <= 0) ? 8 : 4; // lose or respawn after crash

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

void LevelB::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   mGameState.map->render();
   mGameState.witch->render();
   mGameState.enemy->render();
   if (mSecondEnemy != nullptr) mSecondEnemy->render();
   if (carKey != nullptr && carKey->isActive()) carKey->render();

   for (Entity *traffic : mTrafficCars) traffic->render();

   playableCar->render();
   barrier->render();
   redButton->render();

  
}


void LevelB::shutdown()
{
  for (Entity *traffic : mTrafficCars) delete traffic;
  mTrafficCars.clear();
  mTrafficSpeeds.clear();
  delete mGameState.witch;  
  delete mGameState.map;
  delete mGameState.enemy;
  delete mSecondEnemy;
  delete playableCar;
  delete barrier;
  delete carKey;
  delete redButton;

  UnloadMusicStream(mGameState.bgm);
  UnloadSound(mGameState.carStart);
  UnloadSound(mGameState.enemyAttack);
  UnloadSound(mGameState.witchAttack);
  UnloadSound(mGameState.keyCollect);
  UnloadSound(mGameState.barrierOpen);
}

void LevelB::exitCar()
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
