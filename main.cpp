#include <iostream>
#include "window.hpp"
#include "random.hpp"

struct chunk
{
    static constexpr int W = 8;
    static constexpr int H = 8;
    static constexpr int L = 64;

    uint8_t mArray[L];
    uint8_t sym = 0;

    chunk()
    {
        sym = stf::Random().getNum('a', 'z');
        memset(mArray, sym, L);
    }
};

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
