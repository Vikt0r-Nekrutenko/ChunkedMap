#include <iostream>
#include <chrono>
#include "window.hpp"
#include "sdb/chunkedmap.hpp"
#include "sdb/chunk.hpp"

struct Cell : stf::sdb::ICell
{
    uint8_t v;

    Cell(uint8_t s)
        : v{s} { }

    uint8_t view() const
    {
        return v;
    }

    size_t sizeOfSelf() const final
    {
        return sizeof(v);
    }


    void save(FILE *file) final
    {
        fwrite(&v, 1, 1, file);
    }

    void load(FILE *file) final
    {
        fread(&v, 1, 1, file);
    }
};

struct Chunk : public stf::sdb::IChunk
{
    Chunk()
        : IChunk{{8,8}}
    {
        mArray.resize(64);
        uint8_t s = 'a'+rand()%('z'-'a');
        for(auto &c : mArray)
            c = new Cell{(uint8_t)(s)};
    }

};


class Game : public stf::Window
{
    int w = 3000, h = w;
    bool isContinue = true;
    stf::sdb::ChunkedMapT<Chunk> chc = stf::sdb::ChunkedMapT<Chunk>{w,h};
    stf::Vec2d player = {14,19};

public:

    float sec = 0;
    float chunkReloadCounter = 0;
    int delt = 0;
    int avarageTimeToLoadChunk = 0;
    bool isOnPause = 0;

    stf::Vec2f vel = {0,0};
    float px = 13.f, py = 19.f;

    bool onUpdate(const float dt) override
    {
        if(sec > 200.f)
        {
            avarageTimeToLoadChunk = delt / chunkReloadCounter;
            delt = 0;
            chunkReloadCounter = 0;
            sec = 0;
        }
        sec += dt;

        if(isOnPause)
            return isContinue;

        using namespace std::chrono;
        for(int j = 2, y = player.y-13; j < 29; ++j, ++y) {
            for(int i = 1, x = player.x-19; i < 38; ++i, ++x) {
                auto ch = chc[stf::Vec2d{x, y}];
                if(ch != nullptr) {
                    auto t1 = high_resolution_clock::now();

                    Cell *c = static_cast<Cell*>(ch->at({x,y}));
                    renderer.drawPixel(stf::Vec2d{i, j}, c->view());

                    auto t2 = high_resolution_clock::now();
                    delt += duration_cast<microseconds>(t2 - t1).count();
                    ++chunkReloadCounter;
                } else
                    renderer.drawPixel(stf::Vec2d{i, j}, '.');
            }
        }

        chc.cache().update(dt);

        renderer.drawPixel(player - (player - stf::Vec2d{19,13}), 'I', stf::ColorTable::Red);
        renderer.draw({40, 2}, "POS: X[%d]:Y[%d]", player.x, player.y);
        renderer.draw({40, 3}, "VEL: X[%f]:Y[%f]", vel.x, vel.y);


        renderer.draw({40,5}, "New/Del OP : [%d] [%d]", (int)chc.cache().mNewOp, (int)chc.cache().mDelOp);
        renderer.draw({40,6}, "Inp/Outp   : %d", (int)chc.cache().IOCount());
        renderer.draw({40,7}, "Chunks     : %d", (int)chc.cache().cacheSize());
        renderer.draw({40,8}, "Memory     : %fkb", (float)chc.cache().memUsage()/1'000.f);

        renderer.draw({40,9}, "Load T(average) : %dmicS", avarageTimeToLoadChunk);
        renderer.draw({40,10}, "Load T          : %dmicS", chc.cache().mLoadT);

        renderer.draw({40, 12}, "Misses : %d", (int)chc.cache().cacheMisses());
        renderer.draw({40, 13}, "Hits   : %d", (int)chc.cache().cacheHits());
        renderer.draw({40, 14}, "M&S    : %d", (int)chc.cache().cacheCalls());
        renderer.draw({40, 15}, "M/S    : %d", (int)chc.cache().cacheHits() / (int)chc.cache().cacheMisses());

        int y = 17;
        int x = 40;
        for(auto &chunk : chc.cache().mL1) {
            renderer.draw({x, y++}, "%d (%d) [%d]=[%d]---%d:%d",
                          y - 17,
                          chunk.mIsActive,
                          chunk.mLifeTime,
                          chunk.mHits - chunk.mCalls,
                          chunk.mChunkRec.mPos.x,
                          chunk.mChunkRec.mPos.y);
        }


        if(player.x < 1) {
            vel.x = -vel.x;
            px = 1;
        }
        if(player.x > w * Chunk().size().x - 1) {
            vel.x = -vel.x;
            px = (w * Chunk().size().x) - 2;
        }
        if(player.y < 1){
            vel.y = -vel.y;
            py = 1;
        }
        if(player.y > w * Chunk().size().y - 1) {
            vel.y = -vel.y;
            py = (w * Chunk().size().y) - 2;
        }

        px += vel.x * dt;
        py += vel.y * dt;

        player = { px, py };

        return isContinue;
    }

    void keyEvents(const int key) override
    {
        switch (key) {
        case 'w':vel.y -= 0.001f; break;
        case 's':vel.y += 0.001f; break;
        case 'a':vel.x -= 0.001f; break;
        case 'd':vel.x += 0.001f; break;
        case ' ': isOnPause ^= 1; break;
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
