#include "LevelC.h"

class WinMessage : public Scene {
private:
    bool mCondition = false;
    float mTimer = 10.0f;
    bool mSoundPlayed = false;

public:

    WinMessage();
    WinMessage(Vector2 origin, const char *bgHexCode);
    ~WinMessage();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void myDrawText(const char* text, int fontSize, int y_Offset, int x_offset=0, Color color=WHITE);

};