#include "WinMessage.h"

WinMessage::WinMessage()                                      : Scene { {0.0f}, nullptr   } {}
WinMessage::WinMessage(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

WinMessage::~WinMessage() { shutdown(); }

void WinMessage::myDrawText(const char* text, int fontSize, int y_Offset, int x_offset, Color color) {

   int titleWidth = MeasureText(text, fontSize);
   
   int centeredX = (mOrigin.x - titleWidth / 2) + x_offset;

   int centeredY = mOrigin.y + y_Offset;

   DrawText(text, centeredX, centeredY, fontSize, color);
}

void WinMessage::initialise()
{
    mGameState.nextSceneID = -1;
    mGameState.winChime = LoadSound("assets/music_and_sounds/win_chime.mp3");

    /*
      ----------- CAMERA -----------
    */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin; // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
   
   
}

void WinMessage::update(float deltaTime) {

   if (!mSoundPlayed) {
      PlaySound(mGameState.winChime);
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

void WinMessage::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   myDrawText("You Win!", 200, -150, 0, GREEN);
   myDrawText("I was starting to worry, but you did it!", 40, 200, 0, GOLD);
}

void WinMessage::shutdown() {

   UnloadSound(mGameState.winChime);

}

