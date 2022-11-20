#ifndef CHUNKEDMAP_HPP
#define CHUNKEDMAP_HPP

#include "chunk.hpp"

class ChunkedMap
{
    std::list<ChunkRecord> mCache;
    ChunkRecord empty = {{0,0}, new Chunk('\'')};
    const stf::Vec2d Size{0,0};

    const stf::Vec2d mLeftTop;
    const stf::Vec2d mRightBottom;
    const size_t CacheSize = 9;

    const int rightLimEnd = Size.x * Chunk::Width;
    const int rightLim = rightLimEnd - mRightBottom.x;
    const int rightLimBegin = rightLim - mLeftTop.x;

    const int bottomLimEnd = Size.y * Chunk::Height;
    const int bottomLim = bottomLimEnd - mRightBottom.y;
    const int bottomLimBegin = bottomLim - mLeftTop.y;


    ChunkRecord* preload(const stf::Vec2d &pos);

public:


    ChunkedMap(int w, int h, const stf::Vec2d &leftTop, const stf::Vec2d &rightBottom);

    size_t cacheSize() const;
    size_t memUsage() const;
    uint8_t& operator ()(const stf::Vec2d &pos);
    ChunkRecord& operator [](const stf::Vec2d &pos);

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d &camera);
};
#endif // CHUNKEDMAP_HPP
