#include "LevelCTransition.h"

LevelCTransition::LevelCTransition()                                      : Scene { {0.0f}, nullptr   } {}
LevelCTransition::LevelCTransition(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelCTransition::~LevelCTransition() { shutdown(); }

void LevelCTransition::myDrawText(const char* text, int fontSize, int y_Offset, int x_offset, Color color) {

   int titleWidth = MeasureText(text, fontSize);
   
   int centeredX = (mOrigin.x - titleWidth / 2) + x_offset;

   int centeredY = mOrigin.y + y_Offset;

   DrawText(text, centeredX, centeredY, fontSize, color);
}

void LevelCTransition::initialise()
{
    mGameState.nextSceneID = -1;
    mGameState.beware = LoadSound("assets/music_and_sounds/beware_sound.mp3");

    /*
      ----------- CAMERA -----------
    */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin; // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
   
}

void LevelCTransition::update(float deltaTime) {

   if (!mSoundPlayed) {
      PlaySound(mGameState.beware);
      mSoundPlayed = true;
   }
   if (!IsSoundPlaying(mGameState.beware)) mSoundPlayed = false;
   

}

void LevelCTransition::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));



   const char* instructionsText = "Good News and Bad News! The good news is that the enemies\n\n"
                                  "have abandoned their posts for some reason. They left the final\n\n"
                                  "car's key somewhere around it, so now you have to search for it\n\n"
                                  "and find it yourself. The bad news is that it is now dark. You can\n\n"
                                  "barely see anything, so be careful while driving in the dark.\n\n"; 
   

   myDrawText(instructionsText, 30, -300);
   const char* flashlightReminder = "Before I forget, I also gave you a flashlight.\n\n"
                                    "        Press F to toggle it ON or OFF";
   myDrawText(flashlightReminder, 30, 100, 0, YELLOW);
   myDrawText("Press Enter to Complete The Journey!", 50, 250, 0, GREEN);
   
}

void LevelCTransition::shutdown() {

   UnloadSound(mGameState.beware);

}
