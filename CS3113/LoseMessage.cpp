#include "LoseMessage.h"

LoseMessage::LoseMessage()                                      : Scene { {0.0f}, nullptr   } {}
LoseMessage::LoseMessage(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LoseMessage::~LoseMessage() { shutdown(); }

void LoseMessage::myDrawText(const char* text, int fontSize, int y_Offset, int x_offset, Color color) {

   int titleWidth = MeasureText(text, fontSize);
   
   int centeredX = (mOrigin.x - titleWidth / 2) + x_offset;

   int centeredY = mOrigin.y + y_Offset;

   DrawText(text, centeredX, centeredY, fontSize, color);
}

void LoseMessage::initialise()
{
    mGameState.nextSceneID = -1;
    mGameState.loseChime = LoadSound("assets/music_and_sounds/lose_chime.mp3");

    /*
      ----------- CAMERA -----------
    */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin; // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
   
   
}

void LoseMessage::update(float deltaTime) {

   if (!mSoundPlayed) {
      PlaySound(mGameState.loseChime);
      mSoundPlayed = true;
   }

   if (mTimer > 0.0f) {
      mTimer -= deltaTime;
   }
   else {
      mTimer = 10.0f;
      mSoundPlayed = false; 
      mGameState.nextSceneID = 0; 
   }

}

void LoseMessage::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   myDrawText("You Lose!", 200, -150, 0, RED);
   myDrawText("You have failed us all.", 40, 200, 0, WHITE);

}

void LoseMessage::shutdown() {

   UnloadSound(mGameState.loseChime);

}

