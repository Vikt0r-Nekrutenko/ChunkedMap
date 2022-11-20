#include "chunk.hpp"


Chunk::Chunk()
{
    sym = 'a' + rand() % ('z' - 'a');
    memset(mArray, sym, L);
}

Chunk::Chunk(uint8_t s) : sym{s}
{
    memset(mArray, s, L);
}

void Chunk::show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d &posInArray) const
{
    if(posInArray.x < 0 || posInArray.y < 0 || posInArray.x % W > W - 1 || posInArray.y % H > H - 1)
        renderer.drawPixel(posOnScreen, '.');
    else
        renderer.drawPixel(posOnScreen, mArray[W * (posInArray.y % H) + (posInArray.x % W)]);
}

uint8_t &Chunk::operator [](const stf::Vec2d &pos)
{
    return mArray[W * std::abs(pos.y % H) + std::abs(pos.x % W)];
}
