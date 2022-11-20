#include <iostream>
#include <chrono>
#include <thread>
#include "window.hpp"

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
        int indx = W * std::abs(pos.y % H) + std::abs(pos.x % W);
//        if(pos.x < 0 || pos.y < 0 || pos.x % W > W - 1 || pos.y % H > H - 1)
//            throw std::out_of_range(std::to_string(indx));
        return mArray[indx];
    }
};

struct chunkscontroller
{
    struct chunkrecord
    {
        stf::Vec2d pos {0,0};
        chunk *ch {nullptr};
    };

    std::vector<chunkrecord> mChunks;
    std::vector<chunkrecord> mCache;
    chunkrecord mTmpCache;
    chunkrecord empty = {{0,0}, new chunk('.')};
    const stf::Vec2d Size{0,0};

    const stf::Vec2d mLeftTop;
    const stf::Vec2d mRightBottom;

    const int rightLimEnd = Size.x * chunk::W;
    const int rightLim = rightLimEnd - mRightBottom.x;
    const int rightLimBegin = rightLim - mLeftTop.x;

    const int bottomLimEnd = Size.y * chunk::H;
    const int bottomLim = bottomLimEnd - mRightBottom.y;
    const int bottomLimBegin = bottomLim - mLeftTop.y;

    const int cacheW;
    const int cacheH;

    chunkscontroller(int w, int h, const stf::Vec2d &leftTop, const stf::Vec2d &rightBottom)
        : Size{w,h},
          mLeftTop{leftTop},
          mRightBottom{rightBottom},
        cacheW {(leftTop.x + rightBottom.x) / chunk::W},
        cacheH {(leftTop.y + rightBottom.y) / chunk::H}
    {
//        mChunks.resize(w * h, {{0,0},nullptr});

        mCache.resize(cacheW * cacheH);
        FILE *file = std::fopen("chunks.dat", "wb");
        for(int i = 0; i < h; ++i) {
            for(int j = 0; j < w; ++j) {
                stf::Vec2d chunkPos = stf::Vec2d(j,i) * stf::Vec2d(chunk::W, chunk::H);
                uint8_t isNull = true;
                uint8_t chunkmem[sizeof(chunk)];
                memset(chunkmem, 0, sizeof(chunkmem));

                std::fwrite(&isNull, sizeof(uint8_t), 1, file);
                std::fwrite(&chunkPos.x, sizeof(int), 1, file);
                std::fwrite(&chunkPos.y, sizeof(int), 1, file);
                std::fwrite(chunkmem, sizeof(chunkmem), 1, file);
            }
        }
        std::fclose(file);
    }

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen) const
    {
        for(auto &chr : mCache) {
            if(chr.ch != nullptr)
                chr.ch->show(renderer, chr.pos + posOnScreen);
        }
    }

    size_t loadedChunks() const
    {
        size_t n = 0;
        for(auto ch : mCache) {
            if(ch.ch != nullptr)
                ++n;
        }
        return n;
    }

    size_t memUsage() const
    {
        size_t n = 0;
        for(auto ch : mCache) {
            if(ch.ch != nullptr)
                n += sizeof(chunk);
        }
        return n;
    }

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d &camera)
    {
        auto p1 = camera - mLeftTop;
        auto p2 = camera + mRightBottom;

        if(camera.x <= mLeftTop.x){
            p1.x = 0;
            p2.x = mLeftTop.x + mRightBottom.x;
        }
        else if(camera.x >= rightLim) {
            p1.x = rightLimBegin;
            p2.x = rightLimEnd;
        }

        if(camera.y <= mLeftTop.y) {
            p1.y = 0;
            p2.y = mLeftTop.y + mRightBottom.y;
        }
        else if(camera.y >= bottomLim) {
            p1.y = bottomLimBegin;
            p2.y = bottomLimEnd;
        }

//        for(int y = p1.y, sy = posOnScreen.y; y < p2.y; ++y, ++sy) {
//            for(int x = p1.x, sx = posOnScreen.x; x < p2.x; ++x, ++sx) {
//                (*this)[{x,y}].ch->show(renderer, {sx,sy}, {x,y});
//            }
//        }
        for(int y = 0, sy = posOnScreen.y; y < chunk::H; ++y, ++sy) {
            for(int x = 0, sx = posOnScreen.x; x < chunk::W; ++x, ++sx) {
                (*this)[{x,y}].ch->show(renderer, {sx,sy}, {x,y});
            }
        }
    }

    uint8_t& preload(const stf::Vec2d &pos)
    {
        FILE *file = std::fopen("chunks.dat", "r+b");
        size_t seek = sizeof(uint8_t) + sizeof(stf::Vec2d) + sizeof(chunk);

//        for(int y = 0; y < cacheH; ++y) {
//            for(int x = 0; x < cacheW; ++x) {
//                size_t i = cacheW * y + x;
                size_t j = Size.x * pos.y + pos.x;
                fseek(file, j * seek, SEEK_SET);
                uint8_t isNull = 1;
                std::fread(&isNull, sizeof(uint8_t), 1, file);
                std::fread(&mTmpCache.pos, sizeof(stf::Vec2d), 1, file);
//                if(isNull) {
                    mTmpCache.ch = new chunk();
                    isNull = 0;
                    fseek(file, j * seek, SEEK_SET);
                    std::fwrite(&isNull, sizeof(uint8_t), 1, file);
                    std::fseek(file, sizeof(stf::Vec2d), SEEK_CUR);
                    std::fwrite(mTmpCache.ch, sizeof(chunk), 1, file);
//                } else {
//                  std::fread(mTmpCache.ch, sizeof(chunk), 1, file);
//                }
//            }
//        }
        std::fclose(file);
        return (*empty.ch)[pos];
    }

    uint8_t& operator ()(const stf::Vec2d &pos)
    {
        return (*(*this)[pos].ch)[pos];
    }
    chunkrecord& operator [](const stf::Vec2d &pos)
    {
        stf::Vec2d chunkBeginPos = pos / stf::Vec2d(chunk::W, chunk::H);
        stf::Vec2d chunkInCachePos = {pos.x % cacheW, pos.y % cacheH};
//        if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * chunk::W - 1 || pos.y > Size.y * chunk::H - 1)
        if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * chunk::W - 1 || pos.y > Size.y * chunk::H - 1)
//            throw std::out_of_range(std::to_string(Size.x * pos.y + pos.x));
            return empty;
        preload(chunkBeginPos);
//        if(mChunks[Size.x * chunkBeginPos.y + chunkBeginPos.x].ch != nullptr)
//            return mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x];

//        mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x] = {chunkBeginPos, new chunk()};
//        return mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x];
//        if(mCache[Size.x * chunkBeginPos.y + chunkBeginPos.x].ch != nullptr)
//            return mCache[Size.x *  chunkBeginPos.y + chunkBeginPos.x];

//        mCache[Size.x *  chunkBeginPos.y + chunkBeginPos.x] = {chunkBeginPos, new chunk()};
//        return mCache.at(cacheW * chunkInCachePos.y + chunkInCachePos.x).ch == nullptr ? empty : mCache[cacheW *  chunkBeginPos.y + chunkBeginPos.x];
        return mTmpCache.ch == nullptr ? empty : mTmpCache;
    }
};

class Game : public stf::Window
{
    bool isContinue = true;
    chunkscontroller chc{30,30, {5,6}, {5,6}};
    stf::Vec2d player = {0,0};

public:

    Game()
    {
        enableLog(); stf::Renderer::log.setX(40); stf::Renderer::log.setHeight(30);
    }

    bool onUpdate(const float dt) override
    {
        chc.show(renderer, {0,2}, player);
        stf::Renderer::log<<stf::endl<<" Chunks: "<<(int)chc.loadedChunks()<<" mem: "<<(float)chc.memUsage()/1'000'000.f<<"MB"
                            ;
//        chc(player) = 'O';
        return isContinue;
    }

    void keyEvents(const int key) override
    {
//        chc(player);// = chc[player].ch->sym;
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
//    return Game().run(30);
    chunkscontroller chc{30,30, {5,6}, {5,6}};
    std::cout << chc.mTmpCache.pos.x << " " << chc.mTmpCache.pos.x << std::endl;
    std::cout << chc[{12,0}].pos.x << " " << chc[{12,0}].pos.x << std::endl;
    std::cout << chc[{12,0}].ch->sym << std::endl;
}
