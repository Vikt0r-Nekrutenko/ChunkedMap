#include "chunk.hpp"
#include <cstring>
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

Chunk &Chunk::save(FILE *file)
{
    fwrite(mArray, sizeof(mArray), 1, file);
    fwrite(&sym, sizeof(sym), 1, file);
    return *this;
}

Chunk &Chunk::load(FILE *file)
{
    fread(mArray, sizeof(mArray), 1, file);
    fread(&sym, sizeof(sym), 1, file);
    return *this;
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
        mChunk->save(file);
    }
    std::fseek(file, offset * seek + sizeof(uint8_t), SEEK_SET);
    std::fread(&mPos, sizeof(stf::Vec2d), 1, file);
    mChunk->load(file);
    std::fclose(file);
    return *this;
}
