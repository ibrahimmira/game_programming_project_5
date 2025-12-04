#include "Controls.h"

Controls::Controls()                                      : Scene { {0.0f}, nullptr   } {}
Controls::Controls(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

Controls::~Controls() { shutdown(); }

void Controls::myDrawText(const char* text, int fontSize, int y_Offset, int x_offset, Color color) {

   int titleWidth = MeasureText(text, fontSize);
   
   int centeredX = (mOrigin.x - titleWidth / 2) + x_offset;

   int centeredY = mOrigin.y + y_Offset;

   DrawText(text, centeredX, centeredY, fontSize, color);
}

void Controls::initialise()
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

void Controls::update(float deltaTime) {

   UpdateMusicStream(mGameState.menuMusic);

}

void Controls::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));
   
   myDrawText("Movement: \n\n\t\tUp:        W \n\t\tLeft:      A \n\t\tDown:     S \n\t\tRight:     D", 80, -230);
   myDrawText("Press Enter to Continue", 20, 350, 520, GREEN);
}

void Controls::shutdown() {

   UnloadMusicStream(mGameState.menuMusic);

}

