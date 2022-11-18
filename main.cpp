#include <iostream>
#include "window.hpp"

class Game : public stf::Window
{
    bool isContinue = true;

public:
    bool onUpdate(const float dt) override
    {
        return isContinue;
    }

    void keyEvents(const int key) override
    {
        switch (key) {
        case 'q':
            isContinue = false;
            break;
        default:
            break;
        }
    }

    void mouseEvents(const stf::MouseRecord &mr) override
    {

    }
};

int main()
{
    return Game().run();
}
