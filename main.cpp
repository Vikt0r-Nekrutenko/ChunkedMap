#include <iostream>
#include "window.hpp"
#include "chunkedmap.hpp"

class Game : public stf::Window
{
    bool isContinue = true;
    ChunkedMap chc{8,8, {8,8}, {8,8}};
    stf::Vec2d player = {14,14};

public:

    Game()
    {
        enableLog(); stf::Renderer::log.setX(40); stf::Renderer::log.setHeight(30);
    }

    bool onUpdate(const float dt) override
    {
        chc.show(renderer, {0,2}, player);
        stf::Renderer::log<<stf::endl<<"Chunks: "<<(int)chc.cacheSize()<<" mem: "<<(float)chc.memUsage()/1'000.f<<"KB";
        chc(player) = 'O';
        return isContinue;
    }

    void keyEvents(const int key) override
    {
        chc(player) = chc[player].mChunk->sym;
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
    return Game().run(30);
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
//    FILE *f = std::fopen("chunks.dat", "rb");
//    chunkscontroller::chunkrecord rec {{0,0},new chunk('.')};
//    uint8_t isNull = 1;
//    for(int i = 0; i < 64; ++i) {
//        std::fread(&isNull, sizeof(uint8_t), 1, f);
//        std::fread(&rec.pos, sizeof(stf::Vec2d), 1, f);
//        std::fread(rec.ch, sizeof(chunk), 1, f);
//        for(auto y = 0; y < chunk::H; ++y) {
//            for(auto x = 0; x < chunk::W; ++x) {
//                COORD p {(SHORT)(rec.pos.x * chunk::W + x),
//                         (SHORT)(rec.pos.y * chunk::H + y)};
//                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
//                std::cout<<(*rec.ch)[{x,y}];
//            }
//        }
//    }
//    std::fclose(f);
}
