#include "chunk.hpp"


Chunk::Chunk()
{
    sym = 'a' + rand() % ('z' - 'a');
    memset(mArray, sym, Lenght);
}

Chunk::Chunk(uint8_t s) : sym{s}
{
    memset(mArray, s, Lenght);
}

void Chunk::show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d &posInArray) const
{
    if(posInArray.x < 0 || posInArray.y < 0 || posInArray.x % Width > Width - 1 || posInArray.y % Height > Height - 1)
        renderer.drawPixel(posOnScreen, '.');
    else
        renderer.drawPixel(posOnScreen, mArray[Width * (posInArray.y % Height) + (posInArray.x % Width)]);
}

uint8_t &Chunk::operator [](const stf::Vec2d &pos)
{
    return mArray[Width * std::abs(pos.y % Height) + std::abs(pos.x % Width)];
}
