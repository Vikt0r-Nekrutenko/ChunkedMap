#include <iostream>
#include "window.hpp"
#include "chunkedmap.hpp"

class Game : public stf::Window
{
    bool isContinue = true;
    ChunkedMapT<Chunk> chc = ChunkedMapT<Chunk>{8,8};
    stf::Vec2d player = {0,0};

public:

    Game()
    {
        enableLog(); stf::Renderer::log.setX(40); stf::Renderer::log.setHeight(30);
    }

    bool onUpdate(const float dt) override
    {
//        auto cx = player - 4;
        for(int j = 0; j < 8; ++j) {
            for(int i = 0; i < 8; ++i) {
                auto ch = chc[stf::Vec2d{i, j}];
                if(ch != nullptr)
                    renderer.drawPixel({i, j}, ch->at({i,j}).view());
                else
                    renderer.drawPixel({i, j}, '.');
            }
        }
        renderer.drawPixel(player, 'I');
        stf::Renderer::log<<stf::endl<<"Chunks: "<<(int)chc.cacheSize()<<" mem: "<<(float)chc.memUsage()/1'000.f<<"KB";
//        chc(player) = 'O';
        return isContinue;
    }

    void keyEvents(const int key) override
    {
        switch (key) {
        case 'w':player.y--;break;
        case 's':player.y++;break;
        case 'a':player.x--;break;
        case 'd':player.x++;break;
        case ' ':if(chc.put(player, {'W'}) == nullptr) throw; break;
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
