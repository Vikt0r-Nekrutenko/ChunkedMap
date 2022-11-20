#ifndef CHUNKEDMAP_HPP
#define CHUNKEDMAP_HPP

#include "chunk.hpp"

struct ChunkRecord
{
    stf::Vec2d mPos {0,0};
    Chunk *mChunk {nullptr};

    ChunkRecord& load(const char *fileName, const size_t offset);
};

class ChunkedMap
{
    std::list<ChunkRecord> mCache;
    ChunkRecord empty = {{0,0}, new Chunk('\'')};
    const stf::Vec2d Size{0,0};

    const stf::Vec2d mLeftTop;
    const stf::Vec2d mRightBottom;

    const int rightLimEnd = Size.x * Chunk::W;
    const int rightLim = rightLimEnd - mRightBottom.x;
    const int rightLimBegin = rightLim - mLeftTop.x;

    const int bottomLimEnd = Size.y * Chunk::H;
    const int bottomLim = bottomLimEnd - mRightBottom.y;
    const int bottomLimBegin = bottomLim - mLeftTop.y;


    ChunkRecord* preload(const stf::Vec2d &pos);

public:

    size_t CacheSize;

    ChunkedMap(int w, int h, const stf::Vec2d &leftTop, const stf::Vec2d &rightBottom);

    size_t cacheSize() const;
    size_t memUsage() const;
    uint8_t& operator ()(const stf::Vec2d &pos);
    ChunkRecord& operator [](const stf::Vec2d &pos);

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d &camera);
};
#endif // CHUNKEDMAP_HPP
