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
    sym = 'a' + rand() % ('z' - 'a');
    mArray.resize(64, sym);
}

Chunk::Chunk(uint8_t s)
    : IChunk{{8,8}},
    sym{s}
{
    mArray.resize(64, sym);
}

uint8_t &Chunk::operator [](const stf::Vec2d &pos)
{
    return mArray[Size.x * std::abs(pos.y % Size.y) + std::abs(pos.x % Size.x)];
}

size_t Chunk::sizeOfSelf() const
{
    return mArray.size() * sizeof(uint8_t) + sizeof(uint8_t);
}

Chunk &Chunk::save(FILE *file)
{
    fwrite(mArray.data(), mArray.size() * sizeof(uint8_t), 1, file);
    fwrite(&sym, sizeof(sym), 1, file);
    return *this;
}

Chunk &Chunk::load(FILE *file)
{
    fread(mArray.data(), mArray.size() * sizeof(uint8_t), 1, file);
    fread(&sym, sizeof(sym), 1, file);
    return *this;
}
