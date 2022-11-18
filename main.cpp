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

    void show(stf::Renderer &renderer, const stf::Vec2d &pos) const
    {
        for(int y = 0; y < H; ++y) {
            for(int x = 0; x < W; ++x) {
                renderer.drawPixel(pos + stf::Vec2d{x,y}, mArray[W * y + x]);
            }
        }
    }
};

class Game : public stf::Window
{
    bool isContinue = true;

public:
    bool onUpdate(const float dt) override
    {
        chunk().show(renderer, {0,1});
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
