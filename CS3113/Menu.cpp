#include "Menu.h"

Menu::Menu()                                      : Scene { {0.0f}, nullptr   } {}
Menu::Menu(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Menu::~Menu() { shutdown(); }

void Menu::myDrawText(const char* text, int fontSize, int y_Offset, int x_offset, Color color) {

   int titleWidth = MeasureText(text, fontSize);
   
   int centeredX = (mOrigin.x - titleWidth / 2) + x_offset;

   int centeredY = mOrigin.y + y_Offset;

   DrawText(text, centeredX, centeredY, fontSize, color);
}

void Menu::initialise()
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

void Menu::update(float deltaTime) {

   UpdateMusicStream(mGameState.menuMusic);

}

void Menu::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
  
   myDrawText("The Drive", 200, -150, 0, GOLD);
   myDrawText("Press Enter to Start", 50, 200, 0, GREEN);
  
}

void Menu::shutdown() {

   UnloadMusicStream(mGameState.menuMusic);

}

