#include "Menu.h"

class Controls : public Scene {
private:
    bool mCondition = false;

public:

    Controls();
    Controls(Vector2 origin, const char *bgHexCode);
    ~Controls();
    
    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
    void setGameCondition() {mCondition = true;}
    void myDrawText(const char* text, int fontSize, int y_Offset, int x_offset=0, Color color=WHITE);

};