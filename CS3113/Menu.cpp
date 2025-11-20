#include "Menu.h"

Menu::Menu()                                      : Scene { {0.0f}, nullptr   } {}
Menu::Menu(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Menu::~Menu() { shutdown(); }

void Menu::initialise()
{
    mGameState.nextSceneID = -1;

    /*
      ----------- CAMERA -----------
    */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin; // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
   
   
}

void Menu::update(float deltaTime)
{

   if (mCondition) {
      mGameState.nextSceneID = 1;
      mCondition = false;
   }
}

void Menu::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   DrawText(TextFormat("Parking Adventure"), mOrigin.x - 175, mOrigin.y - 70, 40, WHITE);
   DrawText(TextFormat("Press enter to start"), mOrigin.x - 150, mOrigin.y + 50, 30, WHITE);
}

void Menu::shutdown() {}