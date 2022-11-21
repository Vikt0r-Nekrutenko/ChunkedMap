#include "chunk.hpp"
#include <cstring>
#include <cmath>

Chunk::Chunk(const stf::Vec2d &size)
    : Size{size}
{
    sym = 'a' + rand() % ('z' - 'a');
    mArray.resize(size.x * size.y, sym);
}

Chunk::Chunk(const stf::Vec2d &size, uint8_t s)
    : Size{size},
      sym{s}
{
    mArray.resize(size.x * size.y, sym);
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

ChunkRecord &ChunkRecord::load(const char *fileName, const size_t offset)
{
    FILE *file = std::fopen(fileName, "r+b");
    size_t seek = sizeof(uint8_t) + sizeof(stf::Vec2d) + mChunk->sizeOfSelf();
    std::fseek(file, offset * seek, SEEK_SET);
    uint8_t isNull = 1;
    std::fread(&isNull, sizeof(uint8_t), 1, file);

    if(isNull) {
        stf::Vec2d size = mChunk->Size;
        delete mChunk;
        mChunk = new Chunk(size);
        isNull = 0;

        fseek(file, -(long)sizeof(uint8_t), SEEK_CUR);
        std::fwrite(&isNull, sizeof(uint8_t), 1, file);
        std::fseek(file, sizeof(stf::Vec2d), SEEK_CUR);
        mChunk->save(file);
    }
    std::fseek(file, offset * seek + sizeof(uint8_t), SEEK_SET);
    std::fread(&mPos, sizeof(stf::Vec2d), 1, file);
    mChunk->load(file);
    std::fclose(file);
    return *this;
}
