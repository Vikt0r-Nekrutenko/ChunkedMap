#include <iostream>
#include "window.hpp"
#include "chunkedmap.hpp"

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
        for(int j = 0, y = player.y-4; j < 8; ++j, ++y) {
            for(int i = 0, x = player.x-4; i < 8; ++i, ++x) {
                auto ch = chc[stf::Vec2d{x, y}];
                if(ch != nullptr) {
                    Cell *c = static_cast<Cell*>(chc.at({x,y}));
                    renderer.drawPixel(stf::Vec2d{i, j}, c->view());
                } else
                    renderer.drawPixel(stf::Vec2d{i, j}, '.');
            }
        }
        renderer.drawPixel(player - (player - 4), 'I');
        renderer.draw({0, 10}, "%d %d | %d %d", player.x,player.y, camera.x, camera.y);
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
