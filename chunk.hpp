#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "vec2d.hpp"
#include <cstdint>
#include <cstdio>
#include <vector>

struct Chunk
{
    std::vector<uint8_t> mArray;
    const stf::Vec2d Size;
    uint8_t sym = 0;

    Chunk(const stf::Vec2d &size);
    Chunk(const stf::Vec2d &size, uint8_t s);

    uint8_t& operator [](const stf::Vec2d &pos);
    size_t sizeOfSelf() const;
    Chunk& save(FILE *file);
    Chunk& load(FILE *file);
};

struct ChunkRecord
{
    stf::Vec2d mPos {0,0};
    Chunk *mChunk {nullptr};

    ChunkRecord& load(const char *fileName, const size_t offset);
};

#endif // CHUNK_HPP
