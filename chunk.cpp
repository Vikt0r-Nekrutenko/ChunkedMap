#include "chunk.hpp"
#include <cstring>
#include <cmath>

Chunk::Chunk()
    : IChunk{{8,8}}
{
    mArray.resize(64);
    for(auto &c : mArray)
        c = new Cell{(uint8_t)('a'+rand()%('z'-'a'))};
}

IChunk::IChunk(const stf::Vec2d &size)
    : Size{size} { }

IChunk::~IChunk()
{
    for(size_t i = 0; i < mArray.size(); ++i)
        delete mArray.at(i);
}

const stf::Vec2d &IChunk::size() const
{
    return Size;
}

ICell *IChunk::put(const stf::Vec2d &pos, ICell *cell)
{
    return mArray[Size.x * std::abs(pos.y % Size.y) + std::abs(pos.x % Size.x)] = cell;
}

ICell *IChunk::operator [](const stf::Vec2d &pos)
{
    return mArray[Size.x * std::abs(pos.y % Size.y) + std::abs(pos.x % Size.x)];
}

ICell *IChunk::at(const stf::Vec2d &pos)
{
    return mArray[Size.x * std::abs(pos.y % Size.y) + std::abs(pos.x % Size.x)];
}

size_t IChunk::sizeOfSelf() const
{
    size_t size = 0;
    for(auto &c : mArray)
        size += c->sizeOfSelf();
    return mArray.size() * size + sizeof(uint8_t);
}

IChunk &IChunk::save(FILE *file)
{
    for(auto &c : mArray)
        c->save(file);
    return *this;
}

IChunk &IChunk::load(FILE *file)
{
    for(auto &c : mArray)
        c->load(file);
    return *this;
}
