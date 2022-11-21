#include "chunk.hpp"
#include <cstring>
#include <cstdio>
#include <cmath>

Chunk::Chunk()
{
    sym = 'a' + rand() % ('z' - 'a');
    memset(mArray, sym, Lenght);
}

Chunk::Chunk(uint8_t s) : sym{s}
{
    memset(mArray, s, Lenght);
}

uint8_t &Chunk::operator [](const stf::Vec2d &pos)
{
    return mArray[Width * std::abs(pos.y % Height) + std::abs(pos.x % Width)];
}

ChunkRecord &ChunkRecord::load(const char *fileName, const size_t offset)
{
    FILE *file = std::fopen(fileName, "r+b");
    size_t seek = sizeof(uint8_t) + sizeof(stf::Vec2d) + sizeof(Chunk);
    std::fseek(file, offset * seek, SEEK_SET);
    uint8_t isNull = 1;
    std::fread(&isNull, sizeof(uint8_t), 1, file);

    if(isNull) {
        delete mChunk;
        mChunk = new Chunk();
        isNull = 0;

        fseek(file, -(long)sizeof(uint8_t), SEEK_CUR);
        std::fwrite(&isNull, sizeof(uint8_t), 1, file);
        std::fseek(file, sizeof(stf::Vec2d), SEEK_CUR);
        std::fwrite(mChunk, sizeof(Chunk), 1, file);
    }
    std::fseek(file, offset * seek + sizeof(uint8_t), SEEK_SET);
    std::fread(&mPos, sizeof(stf::Vec2d), 1, file);
    std::fread(mChunk, sizeof(Chunk), 1, file);
    std::fclose(file);
    return *this;
}
