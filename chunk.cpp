#include "chunk.hpp"
#include <cstring>
#include <cmath>

IChunk::IChunk(const stf::Vec2d &size)
    : Size{size} { }

const stf::Vec2d &IChunk::size() const
{
    return Size;
}

Chunk::Chunk()
    : IChunk{{8,8}}
{
    mArray.resize(64, {(uint8_t)('a'+rand()%('z'-'a'))});
}

Cell &Chunk::operator [](const stf::Vec2d &pos)
{
    return mArray[Size.x * std::abs(pos.y % Size.y) + std::abs(pos.x % Size.x)];
}

Cell &Chunk::at(const stf::Vec2d &pos)
{
    return mArray[Size.x * std::abs(pos.y % Size.y) + std::abs(pos.x % Size.x)];
}

size_t Chunk::sizeOfSelf() const
{
    return mArray.size() * sizeof(Cell) + sizeof(uint8_t);
}

Chunk &Chunk::save(FILE *file)
{
    for(auto &c : mArray)
        c.save(file);
    fseek(file, sizeof(uint8_t), SEEK_CUR);
    return *this;
}

Chunk &Chunk::load(FILE *file)
{
    for(auto &c : mArray)
        c.load(file);
    fseek(file, sizeof(uint8_t), SEEK_CUR);
    return *this;
}
