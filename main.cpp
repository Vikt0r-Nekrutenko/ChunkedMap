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
    chunk(uint8_t s) : sym{s}
    {
        memset(mArray, s, L);
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
        if(posInArray.x < 0 || posInArray.y < 0 || posInArray.x % W > W - 1 || posInArray.y % H > H - 1)
            renderer.drawPixel(posOnScreen, '.');
        else
            renderer.drawPixel(posOnScreen, mArray[W * (posInArray.y % H) + (posInArray.x % W)]);
    }

    uint8_t& operator [](const stf::Vec2d &pos)
    {
        int indx = W * (pos.y % H) + (pos.x % W);
        if(pos.x < 0 || pos.y < 0 || pos.x % W > W - 1 || pos.y % H > H - 1)
            throw std::out_of_range(std::to_string(indx));
        return mArray[indx];
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
    chunkrecord empty = {{0,0}, new chunk('.')};
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

    uint8_t& operator ()(const stf::Vec2d &pos)
    {
        return (*(*this)[pos].ch)[pos];
    }
    chunkrecord& operator [](const stf::Vec2d &pos)
    {
        stf::Vec2d chunkBeginPos = pos / stf::Vec2d(chunk::W, chunk::H);
        if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * chunk::W - 1 || pos.y > Size.y * chunk::H - 1)
//            throw std::out_of_range(std::to_string(Size.x * pos.y + pos.x));
            return empty;

        if(mChunks[Size.x * chunkBeginPos.y + chunkBeginPos.x].ch != nullptr)
            return mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x];

        mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x] = {chunkBeginPos, new chunk()};
        return mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x];
    }
};

class Game : public stf::Window
{
    bool isContinue = true;
    chunkscontroller chc{8,8};
    stf::Vec2d player = {0,0};

public:
    bool onUpdate(const float dt) override
    {
        chc(player) = 'O';
        chc.show(renderer, {0,2}, player-stf::Vec2d{4,4}, player+stf::Vec2d{5,5});
        return isContinue;
    }

    void keyEvents(const int key) override
    {
        chc(player) = chc[player].ch->sym;
        switch (key) {
        case 'w':player.y--;break;
        case 's':player.y++;break;
        case 'a':player.x--;break;
        case 'd':player.x++;break;
        case 'q':isContinue = false;break;
        default:break;
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
