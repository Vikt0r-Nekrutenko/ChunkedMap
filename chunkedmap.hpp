#ifndef CHUNKEDMAP_HPP
#define CHUNKEDMAP_HPP

#include "chunk.hpp"
#include <list>
#include <cstring>

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

template<class ChunkT>class ChunkedMapT
{
    std::list<ChunkRecordT<ChunkT>> mCache;
    std::string mChunksFileName;
    ChunkRecordT<ChunkT> empty;
    const stf::Vec2d Size;
    const stf::Vec2d ChunkSize;
    const size_t CacheSize = 9;

public:
    ChunkedMapT(int w, int h, const stf::Vec2d &chunkSize, const std::string &fileName = "chunks.dat")
        : mChunksFileName{fileName},
          empty {{0,0}, new ChunkT(chunkSize, '\'')},
          Size{w,h},
          ChunkSize{chunkSize}
    {
        FILE *fileIsExist = std::fopen(mChunksFileName.c_str(), "r+b");
        if(fileIsExist == (FILE*)false) {
            FILE *file = std::fopen(mChunksFileName.c_str(), "wb");
            for(int i = 0; i < h; ++i) {
                for(int j = 0; j < w; ++j) {
                    stf::Vec2d chunkPos = stf::Vec2d(j,i);
                    uint8_t isNull = true;
                    uint8_t chunkmem[ChunkT(chunkSize).sizeOfSelf()];
                    memset(chunkmem, '.', sizeof(chunkmem));

                    std::fwrite(&isNull, sizeof(uint8_t), 1, file);
                    std::fwrite(&chunkPos, sizeof(stf::Vec2d), 1, file);
                    std::fwrite(chunkmem, sizeof(chunkmem), 1, file);
                }
            }
            std::fclose(file);
        }
        std::fclose(fileIsExist);
    }

    size_t cacheSize() const
    {
        return mCache.size();
    }

    size_t memUsage() const
    {

        return mCache.size() * ChunkT(ChunkSize).sizeOfSelf() + sizeof(stf::Vec2d);
    }

    ChunkRecordT<ChunkT> *preload(const stf::Vec2d &pos)
    {
        auto selected = &empty;
        for(auto &i : mCache) { if(i.mPos == pos) { selected = &i; break; }}

        if(selected->mChunk->sym == empty.mChunk->sym) {
            if(mCache.size() >= CacheSize) {
                delete mCache.front().mChunk;
                mCache.pop_front();
            }
            mCache.push_back({{0,0}, new ChunkT(ChunkSize, '#')});
            size_t offset = Size.x * pos.y + pos.x;
            return &mCache.back().load(mChunksFileName, offset);
        }
        return selected;
    }

    uint8_t &operator ()(const stf::Vec2d &pos)
    {
        return (*(*this)[pos].mChunk)[pos];
    }

    ChunkRecordT<ChunkT> &operator [](const stf::Vec2d &pos)
    {
        stf::Vec2d chunkBeginPos = pos / stf::Vec2d(ChunkSize.x, ChunkSize.y);
        if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * ChunkSize.x - 1 || pos.y > Size.y * ChunkSize.y - 1)
            return empty;
        return *preload(chunkBeginPos);
    }
};
#endif // CHUNKEDMAP_HPP
