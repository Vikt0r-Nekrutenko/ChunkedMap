#ifndef CHUNKEDMAP_HPP
#define CHUNKEDMAP_HPP

#include "chunk.hpp"

class ChunkedMap
{
    std::list<ChunkRecord> mCache;
    ChunkRecord empty = {{0,0}, new Chunk('\'')};
    const stf::Vec2d Size{0,0};
    const size_t CacheSize = 9;
    ChunkRecord* preload(const stf::Vec2d &pos);

public:


    ChunkedMap(int w, int h);

    size_t cacheSize() const;
    size_t memUsage() const;
    uint8_t& operator ()(const stf::Vec2d &pos);
    ChunkRecord& operator [](const stf::Vec2d &pos);
};
#endif // CHUNKEDMAP_HPP
