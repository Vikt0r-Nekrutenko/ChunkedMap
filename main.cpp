#include <iostream>
#include "window.hpp"
#include "chunkedmap.hpp"

class Game : public stf::Window
{
    bool isContinue = true;
    ChunkedMap chc{8,8, {8,8}};
    stf::Vec2d player = {0,0};

public:

    Game()
    {
        enableLog(); stf::Renderer::log.setX(40); stf::Renderer::log.setHeight(30);
    }

    bool onUpdate(const float dt) override
    {
        auto cx = player - 4;
        for(int j = 0; j < 8; ++j) {
            for(int i = 0; i < 8; ++i) {
                renderer.drawPixel({i, j}, chc(stf::Vec2d{i, j}+cx));
            }
        }
        renderer.drawPixel(player - cx, 'I');
        stf::Renderer::log<<stf::endl<<"Chunks: "<<(int)chc.cacheSize()<<" mem: "<<(float)chc.memUsage()/1'000.f<<"KB";
//        chc(player) = 'O';
        return isContinue;
    }

    void keyEvents(const int key) override
    {
//        chc(player) = chc[player].mChunk->sym;
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
//    std::cout << Chunk({8,8}).sizeOfSelf();
}
