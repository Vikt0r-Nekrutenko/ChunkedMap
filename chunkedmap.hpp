#ifndef CHUNKEDMAP_HPP
#define CHUNKEDMAP_HPP

#include "chunk.hpp"
#include <list>
#include <cstring>

template<class IChunkT> class ChunkedMapT
{
    std::list<ChunkRecordT<IChunkT>> mCache;
    std::string mChunksFileName;
    const stf::Vec2d Size;
    const size_t CacheSize = 9;

public:

    ChunkedMapT(int w, int h, const std::string &fileName = "chunks.dat")
        : mChunksFileName{fileName},
          Size{w,h}
    {
        FILE *fileIsExist = std::fopen(mChunksFileName.c_str(), "r+b");
        if(fileIsExist == (FILE*)false) {
            FILE *file = std::fopen(mChunksFileName.c_str(), "wb");
            for(int y = 0; y < h; ++y) {
                for(int x = 0; x < w; ++x) {
                    stf::Vec2d chunkPos = stf::Vec2d(x,y);
                    uint8_t isNull = true;
                    uint8_t chunkmem[IChunkT().sizeOfSelf()];
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

        return mCache.size() * IChunkT().sizeOfSelf() + sizeof(stf::Vec2d);
    }


    IChunkT *put(const stf::Vec2d &pos, ICell *cell)
    {
        stf::Vec2d chunkBeginPos = pos / stf::Vec2d(IChunkT().size().x, IChunkT().size().y);
        size_t offset = Size.x * chunkBeginPos.y + chunkBeginPos.x;
        for(ChunkRecordT<IChunkT> &i : mCache) {
            if(i.mPos == chunkBeginPos) {
                i.mChunk->put(pos, cell);
                i.save(mChunksFileName, offset);
                return i.mChunk;
        }}
        return nullptr;
    }

    IChunkT *operator [](const stf::Vec2d &pos)
    {
        stf::Vec2d chunkBeginPos = pos / stf::Vec2d(IChunkT().size().x, IChunkT().size().y);
        if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * IChunkT().size().x - 1 || pos.y > Size.y * IChunkT().size().y - 1)
            return nullptr;
        return preload(chunkBeginPos)->mChunk;
    }

    ICell *at(const stf::Vec2d &pos)
    {
        IChunkT *chunk = (*this)[pos];
        if(chunk != nullptr)
            return chunk->at({pos});
        return nullptr;
    }

private:

    ChunkRecordT<IChunkT> *preload(const stf::Vec2d &pos)
    {
        for(auto &i : mCache) { if(i.mPos == pos) { return &i; }}

        if(mCache.size() >= CacheSize) {
            delete mCache.front().mChunk;
            mCache.pop_front();
        }
        mCache.push_back({{0,0}, new IChunkT});
        size_t offset = Size.x * pos.y + pos.x;
        return &mCache.back().load(mChunksFileName, offset);
    }
};
#endif // CHUNKEDMAP_HPP
