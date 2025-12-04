#include "Controls.h"

class GameInstructions : public Scene {
private:
    bool mCondition = false;

public:

    GameInstructions();
    GameInstructions(Vector2 origin, const char *bgHexCode);
    ~GameInstructions();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void setGameCondition() {mCondition = true;}
    void myDrawText(const char* text, int fontSize, int y_Offset, int x_offset=0, Color color=WHITE);

};