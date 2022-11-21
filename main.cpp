#include <iostream>
#include "window.hpp"
#include "chunkedmap.hpp"

struct Cell : ICell
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

struct Chunk : public IChunk
{
    Chunk()
        : IChunk{{8,8}}
    {
        mArray.resize(64);
        for(auto &c : mArray)
            c = new Cell{(uint8_t)('a'+rand()%('z'-'a'))};
    }

};


class Game : public stf::Window
{
    bool isContinue = true;
    ChunkedMapT<Chunk> chc = ChunkedMapT<Chunk>{8,8};
    stf::Vec2d player = {4,4};

public:

    Game()
    {
        enableLog(); stf::Renderer::log.setX(40); stf::Renderer::log.setHeight(30);
    }
    bool onUpdate(const float dt) override
    {
        for(int j = 0, y = player.y-4; j < 9; ++j, ++y) {
            for(int i = 0, x = player.x-4; i < 9; ++i, ++x) {
                auto ch = chc[stf::Vec2d{x, y}];
                if(ch != nullptr) {
                    Cell *c = static_cast<Cell*>(chc.at({x,y}));
                    renderer.drawPixel(stf::Vec2d{i, j}, c->view());
                } else
                    renderer.drawPixel(stf::Vec2d{i, j}, '.');
            }
        }
        renderer.drawPixel(player - (player - 4), 'I');
        renderer.draw({0, 10}, "%d %d", player.x,player.y);
        stf::Renderer::log<<stf::endl<<"Chunks: "<<(int)chc.cacheSize()<<" mem: "<<(float)chc.memUsage()/1'000.f<<"KB";
        return isContinue;
    }

    void keyEvents(const int key) override
    {
        switch (key) {
        case 'w':player.y--; break;
        case 's':player.y++; break;
        case 'a':player.x--; break;
        case 'd':player.x++; break;
        case ' ':chc.put(player, new Cell('W')); break;
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
//    std::cout << Chunk({8,8}).sizeOfSelf();
}
