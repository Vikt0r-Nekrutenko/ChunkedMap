#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "renderer.hpp"

struct Chunk
{
    static constexpr int Width = 8;
    static constexpr int Height = Width;
    static constexpr int Lenght = Width * Height;

    uint8_t mArray[Lenght];
    uint8_t sym = 0;

    Chunk();
    Chunk(uint8_t s);

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d& posInArray) const;
    uint8_t& operator [](const stf::Vec2d &pos);
};

struct ChunkRecord
{
    stf::Vec2d mPos {0,0};
    Chunk *mChunk {nullptr};

    ChunkRecord& load(const char *fileName, const size_t offset);
};

#endif // CHUNK_HPP