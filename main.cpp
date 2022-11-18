#include <iostream>
#include <chrono>
#include "window.hpp"
#include "random.hpp"

using namespace std::chrono;

struct chunk
{
    static constexpr int W = 3;
    static constexpr int H = W;
    static constexpr int L = W * H;

    uint8_t mArray[L];
    uint8_t sym = 0;

    chunk()
    {
        sym = 'a' + rand() % ('z' - 'a');
        memset(mArray, sym, L);
    }

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen) const
    {
        for(int y = 0; y < H; ++y) {
            for(int x = 0; x < W; ++x) {
                renderer.drawPixel(posOnScreen * W + stf::Vec2d{x,y}, mArray[W * y + x]);
            }
        }
    }
    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d& posInArray) const
    {
        renderer.drawPixel(posOnScreen, mArray[W * (posInArray.y % H) + (posInArray.x % W)]);
    }

    uint8_t& operator [](const stf::Vec2d &pos)
    {
        return mArray[W * (pos.y % H) + (pos.x % W)];
    }
};

struct chunkscontroller
{
    struct chunkrecord
    {
        stf::Vec2d pos;
        chunk *ch;
    };

    std::vector<chunkrecord> mChunks;
    const stf::Vec2d Size{0,0};

    chunkscontroller(int w, int h) : Size{w,h}
    {
        mChunks.resize(w * h, {{0,0},nullptr});
    }

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen) const
    {
        for(auto &chr : mChunks) {
            if(chr.ch != nullptr)
                chr.ch->show(renderer, chr.pos + posOnScreen);
        }
    }

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d &p1, const stf::Vec2d &p2)
    {
        for(int y = p1.y, sy = posOnScreen.y; y < p2.y; ++y, ++sy) {
            for(int x = p1.x, sx = posOnScreen.x; x < p2.x; ++x, ++sx) {
                (*this)[{x,y}].ch->show(renderer, {sx,sy}, {x,y});
            }
        }
    }

    chunkrecord& operator [](const stf::Vec2d &pos)
    {
        stf::Vec2d chunkBeginPos = pos / stf::Vec2d(chunk::W, chunk::H);
        if(mChunks[Size.x * chunkBeginPos.y + chunkBeginPos.x].ch == nullptr)
            mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x] = {chunkBeginPos, new chunk()};
        return mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x];
    }
};

class Game : public stf::Window
{
    bool isContinue = true;
    chunkscontroller chc{24,24};

public:
    bool onUpdate(const float dt) override
    {
        (*chc[{3,3}].ch)[{3,3}] = 'O';
        (*chc[{15,3}].ch)[{15,3}] = 'O';
        chc.show(renderer, {0,2}, {1,1}, {18,4});
//        chc.show(renderer, {1,1}, {25,25});
//        chc.show(renderer, {0,1});
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
