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
    chunkrecord mTmpCache {{0,0}, new chunk('_')};
    chunkrecord empty = {{0,0}, new chunk('_')};
    const stf::Vec2d Size{0,0};

    const stf::Vec2d mLeftTop;
    const stf::Vec2d mRightBottom;

    stf::Vec2d mCamLeftTop;
    stf::Vec2d mCamRightBottom;

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
          mCamLeftTop {leftTop},
          mCamRightBottom {rightBottom},
        cacheW {(leftTop.x + rightBottom.x) / chunk::W},
        cacheH {(leftTop.y + rightBottom.y) / chunk::H}
    {
//        mChunks.resize(w * h, {{0,0},nullptr});

//        mCache.resize(cacheW * cacheH);
//        for(auto &i : mCache)
//            i.ch = new chunk;

//        FILE *file = std::fopen("chunks.dat", "wb");
//        for(int i = 0; i < h; ++i) {
//            for(int j = 0; j < w; ++j) {
//                stf::Vec2d chunkPos = stf::Vec2d(j,i);// * stf::Vec2d(chunk::W, chunk::H);
//                uint8_t isNull = true;
//                uint8_t chunkmem[sizeof(chunk)];
//                memset(chunkmem, '.', sizeof(chunkmem));

//                std::fwrite(&isNull, sizeof(uint8_t), 1, file);
//                std::fwrite(&chunkPos.x, sizeof(int), 1, file);
//                std::fwrite(&chunkPos.y, sizeof(int), 1, file);
////                std::fwrite(mChunks.at(w*j+i).ch, sizeof(chunk), 1, file);
//                std::fwrite(chunkmem, sizeof(chunkmem), 1, file);
//            }
//        }
//        std::fclose(file);
    }

    size_t memUsage() const
    {
        size_t n = 0;
        for(auto ch : mCache) {
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

        for(int y = p1.y, sy = posOnScreen.y; y < p2.y; y++, sy++) {
            for(int x = p1.x, sx = posOnScreen.x; x < p2.x; x++, sx++) {
                (*this)[{x,y}].ch->show(renderer, {sx,sy}, {x,y});
            }
        }
    }

    chunkrecord* preload(const stf::Vec2d &pos)
    {
        size_t seek = sizeof(uint8_t) + sizeof(stf::Vec2d) + sizeof(chunk);
        auto findPos = [&]() -> chunkrecord* {
            for(auto &i : mCache) {
                if(i.pos == pos)
                    return &i;
            }
            return &empty;
        };
        auto ch = findPos();
        if(ch->ch->sym == empty.ch->sym) {
            FILE *file = std::fopen("chunks.dat", "r+b");
            mCache.push_back({{0,0}, new chunk('#')});

            size_t j = Size.x * pos.y + pos.x;
            fseek(file, j * seek, SEEK_SET);
            uint8_t isNull = 1;
            std::fread(&isNull, sizeof(uint8_t), 1, file);
            std::fread(&mCache.back().pos, sizeof(stf::Vec2d), 1, file);
stf::Renderer::log<<stf::endl<<mCache.back().pos;
            if(isNull) {
                delete mCache.back().ch;
                mCache.back().ch = new chunk();
                isNull = 0;
                fseek(file, j * seek, SEEK_SET);
                std::fwrite(&isNull, sizeof(uint8_t), 1, file);
                std::fseek(file, sizeof(stf::Vec2d), SEEK_CUR);
                std::fwrite(mCache.back().ch, sizeof(chunk), 1, file);
            }
            fseek(file, j * seek + sizeof(uint8_t) + sizeof(stf::Vec2d), SEEK_SET);
            std::fread(mCache.back().ch, sizeof(chunk), 1, file);
            std::fclose(file);
            return &mCache.back();
        }
        return ch;
    }

    uint8_t& operator ()(const stf::Vec2d &pos)
    {
        return (*(*this)[pos].ch)[pos];
    }
    chunkrecord& operator [](const stf::Vec2d &pos)
    {
        stf::Vec2d chunkBeginPos = pos / stf::Vec2d(chunk::W, chunk::H);
//        stf::Vec2d chunkInCachePos = {pos.x % cacheW, pos.y % cacheH};
//        if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * chunk::W - 1 || pos.y > Size.y * chunk::H - 1)
        if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * chunk::W - 1 || pos.y > Size.y * chunk::H - 1)
//            throw std::out_of_range(std::to_string(Size.x * pos.y + pos.x));
            return empty;
        return *preload(chunkBeginPos);
//        if(mChunks[Size.x * chunkBeginPos.y + chunkBeginPos.x].ch != nullptr)
//            return mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x];

//        mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x] = {chunkBeginPos, new chunk()};
//        return mChunks[Size.x *  chunkBeginPos.y + chunkBeginPos.x];
//        if(mCache[Size.x * chunkBeginPos.y + chunkBeginPos.x].ch != nullptr)
//            return mCache[Size.x *  chunkBeginPos.y + chunkBeginPos.x];

//        mCache[Size.x *  chunkBeginPos.y + chunkBeginPos.x] = {chunkBeginPos, new chunk()};
//        return mCache.at(cacheW * chunkInCachePos.y + chunkInCachePos.x).ch == nullptr ? empty : mCache[cacheW *  chunkBeginPos.y + chunkBeginPos.x];
//        return mTmpCache;
    }
};

class Game : public stf::Window
{
    bool isContinue = true;
    chunkscontroller chc{8,8, {3,3}, {3,3}};
    stf::Vec2d player = {14,14};

public:

    Game()
    {
        enableLog(); stf::Renderer::log.setX(40); stf::Renderer::log.setHeight(30);
    }

    bool onUpdate(const float dt) override
    {
        chc.show(renderer, {0,2}, player);
//        stf::Renderer::log<<stf::endl<<" Chunks: "<<(int)chc.mCache.size()<<" mem: "<<(float)chc.memUsage()/1'000.f<<"KB";
//        stf::Renderer::log<<stf::endl<< chc.mCamLeftTop-(chc.mTmpCache.pos)<<" "<<chc.mCamRightBottom+chc.mTmpCache.pos;
//        chc(player) = 'O';
        return isContinue;
    }

    void keyEvents(const int key) override
    {
//        chc(player) = chc[player].ch->sym;
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
//    stf::Renderer r({31,31});
//    chunkscontroller chc{8,8, {3,3}, {3,3}};
//    chc.show(r,{0,2},{2,2});
//    std::cout << chc.mCache.at(0).pos.x << " " << chc.mTmpCache.pos.y << std::endl;
//    if(chc[{1,0}].ch == nullptr)
//        std::cout<<"NULL"<<std::endl;
//    else {
//    std::cout << chc[{3,0}].pos.x << " " << chc[{1,0}].pos.y << std::endl;
//    std::cout << chc[{3,0}].ch->sym << std::endl;
//    }
    FILE *f = std::fopen("chunks.dat", "rb");
    chunkscontroller::chunkrecord rec {{0,0},new chunk('.')};
    uint8_t isNull = 1;
    for(int i = 0; i < 64; ++i) {
        std::fread(&isNull, sizeof(uint8_t), 1, f);
        std::fread(&rec.pos, sizeof(stf::Vec2d), 1, f);
        std::fread(rec.ch, sizeof(chunk), 1, f);
        for(auto y = 0; y < chunk::H; ++y) {
            for(auto x = 0; x < chunk::W; ++x) {
                COORD p {(SHORT)(rec.pos.x * chunk::W + x),
                         (SHORT)(rec.pos.y * chunk::H + y)};
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
                std::cout<<(*rec.ch)[{x,y}];
            }
        }
    }
    std::fclose(f);
}
