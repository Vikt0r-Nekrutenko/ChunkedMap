#ifndef CHUNKEDMAP_HPP
#define CHUNKEDMAP_HPP

#include "chunk.hpp"
#include <list>

class ChunkedMap
{
    std::list<ChunkRecord> mCache;
    ChunkRecord empty;
    const stf::Vec2d Size;
    const stf::Vec2d ChunkSize;
    const size_t CacheSize = 9;
    ChunkRecord* preload(const stf::Vec2d &pos);

public:


    ChunkedMap(int w, int h, const stf::Vec2d &chunkSize);

    size_t cacheSize() const;
    size_t memUsage() const;
    uint8_t& operator ()(const stf::Vec2d &pos);
    ChunkRecord& operator [](const stf::Vec2d &pos);
};
#endif // CHUNKEDMAP_HPP
