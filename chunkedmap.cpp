#include <cstring>
#include "chunkedmap.hpp"


ChunkedMap::ChunkedMap(int w, int h, const stf::Vec2d &chunkSize)
    : empty {{0,0}, new Chunk(chunkSize, '\'')},
      Size{w,h},
      ChunkSize{chunkSize}
{
    FILE *fileIsExist = std::fopen("chunks.dat", "r+b");
    if(fileIsExist == (FILE*)false) {
        FILE *file = std::fopen("chunks.dat", "wb");
        for(int i = 0; i < h; ++i) {
            for(int j = 0; j < w; ++j) {
                stf::Vec2d chunkPos = stf::Vec2d(j,i);
                uint8_t isNull = true;
                uint8_t chunkmem[Chunk(chunkSize).sizeOfSelf()];
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

size_t ChunkedMap::cacheSize() const
{
    return mCache.size();
}

size_t ChunkedMap::memUsage() const
{

    return mCache.size() * Chunk(ChunkSize).sizeOfSelf() + sizeof(stf::Vec2d);
}

ChunkRecord *ChunkedMap::preload(const stf::Vec2d &pos)
{
    auto selected = &empty;
    for(auto &i : mCache) { if(i.mPos == pos) { selected = &i; break; }}

    if(selected->mChunk->sym == empty.mChunk->sym) {
        if(mCache.size() >= CacheSize) {
            delete mCache.front().mChunk;
            mCache.pop_front();
        }
        mCache.push_back({{0,0}, new Chunk(ChunkSize, '#')});
        size_t offset = Size.x * pos.y + pos.x;
        return &mCache.back().load("chunks.dat", offset);
    }
    return selected;
}

uint8_t &ChunkedMap::operator ()(const stf::Vec2d &pos)
{
    return (*(*this)[pos].mChunk)[pos];
}

ChunkRecord &ChunkedMap::operator [](const stf::Vec2d &pos)
{
    stf::Vec2d chunkBeginPos = pos / stf::Vec2d(ChunkSize.x, ChunkSize.y);
    if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * ChunkSize.x - 1 || pos.y > Size.y * ChunkSize.y - 1)
        return empty;
    return *preload(chunkBeginPos);
}
