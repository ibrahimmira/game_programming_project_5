#include "GameInstructions.h"

GameInstructions::GameInstructions()                                      : Scene { {0.0f}, nullptr   } {}
GameInstructions::GameInstructions(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

GameInstructions::~GameInstructions() { shutdown(); }

void GameInstructions::myDrawText(const char* text, int fontSize, int y_Offset, int x_offset, Color color) {

   int titleWidth = MeasureText(text, fontSize);
   
   int centeredX = (mOrigin.x - titleWidth / 2) + x_offset;

   int centeredY = mOrigin.y + y_Offset;

   DrawText(text, centeredX, centeredY, fontSize, color);
}

void GameInstructions::initialise()
{
    mGameState.nextSceneID = -1;
    mGameState.menuMusic = LoadMusicStream("assets/music_and_sounds/menu_music.mp3");
    PlayMusicStream(mGameState.menuMusic);

    /*
      ----------- CAMERA -----------
    */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin; // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
   
   
}

void GameInstructions::update(float deltaTime) {

   UpdateMusicStream(mGameState.menuMusic);

}

void GameInstructions::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));


   // const char* instructionsText = "You have been tasked to move valuable cars from point A to B.\n\n"
   //                                "You are also a witch! That is percisely why you were selected for this job.\n\n"
   //                                "You are more of capable than doing it. You will find enemies on your way, \n\n"
   //                                "if you get close to them, they will attack. Unfortunately, they usually \n\n"
   //                                "have what you want. Whether that is the valuable cars' keys or the barrier \n\n"
   //                                "controllers. Defeat them to collect these items."; 
   const char* instructionsText = "You have been tasked with moving valuable cars from point A to B.\n\n"
                                  "You are also a highly specialized, highly skilled individual. That is\n\n"
                                  "precisely why you were selected for this job. You are more than capable\n\n"
                                  "of doing it. You will find enemies on your way, if you get close to\n\n"
                                  "them, they will attack. Unfortunately, they usually have what you need,\n\n"
                                  "whether that is the valuable cars' keys or the barrier controllers.\n\n"
                                  "Defeat them to collect these items. Oh, and DON'T crash into moving traffic!\n\n"; 
   
   // myDrawText(instructionsText, 30, -250);
   myDrawText(instructionsText, 30, -300);
   myDrawText("Press Enter to Start Your Journey!", 50, 250, 0, GREEN);
   // myDrawText("You can use J for attack, and Good Luck!", 30, 150, 0, YELLOW);
   myDrawText("You can use J for attack, and Good Luck!", 30, 170, 0, YELLOW);
   
}

void GameInstructions::shutdown() {

   UnloadMusicStream(mGameState.menuMusic);

}

