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

    uint8_t& operator [](const stf::Vec2d &pos)
    {
        return mArray[W * (pos.y % H) + (pos.x % W)];
    }
};

struct chunkscontroller
{
    std::vector<chunk> mChunks;
    const stf::Vec2d Size{0,0};

    chunkscontroller(int w, int h) : Size{w,h}
    {

    }

    void show(stf::Renderer &renderer, const stf::Vec2d &pos) const
    {

    }
};

class Game : public stf::Window
{
    bool isContinue = true;
    chunk ch;

public:
    bool onUpdate(const float dt) override
    {
        ch[{3,3}] = 'O';
        ch.show(renderer, {0,1});
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
