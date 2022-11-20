#include "chunkedmap.hpp"


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
        std::fwrite(mChunk, sizeof(Chunk), 1, file);
    }
    std::fseek(file, offset * seek + sizeof(uint8_t), SEEK_SET);
    std::fread(&mPos, sizeof(stf::Vec2d), 1, file);
    std::fread(mChunk, sizeof(Chunk), 1, file);
    std::fclose(file);
    return *this;
}

ChunkedMap::ChunkedMap(int w, int h, const stf::Vec2d &leftTop, const stf::Vec2d &rightBottom)
    : Size{w,h},
      mLeftTop{leftTop},
      mRightBottom{rightBottom},
      CacheSize(((leftTop.x + rightBottom.x)) + ((leftTop.y + rightBottom.y)))
{
    FILE *fileIsExist = std::fopen("chunks.dat", "r+b");
    if(fileIsExist == (FILE*)false) {
        FILE *file = std::fopen("chunks.dat", "wb");
        for(int i = 0; i < h; ++i) {
            for(int j = 0; j < w; ++j) {
                stf::Vec2d chunkPos = stf::Vec2d(j,i);
                uint8_t isNull = true;
                uint8_t chunkmem[sizeof(Chunk)];
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
    size_t n = 0;
    for(auto ch : mCache) {
        n += sizeof(Chunk);
    }
    return n;
}

void ChunkedMap::show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d &camera)
{
    auto p1 = camera - mLeftTop;
    auto p2 = camera + mRightBottom;

    if(camera.x <= mLeftTop.x){
        p1.x = 0;
        p2.x = mLeftTop.x + mRightBottom.x;
    }
    else if(camera.x >= rightLim) {
        p1.x = rightLimBegin;
        p2.x = rightLimEnd;
    }

    if(camera.y <= mLeftTop.y) {
        p1.y = 0;
        p2.y = mLeftTop.y + mRightBottom.y;
    }
    else if(camera.y >= bottomLim) {
        p1.y = bottomLimBegin;
        p2.y = bottomLimEnd;
    }

    for(int y = p1.y, sy = posOnScreen.y; y < p2.y; y++, sy++) {
        for(int x = p1.x, sx = posOnScreen.x; x < p2.x; x++, sx++) {
            (*this)[{x,y}].mChunk->show(renderer, {sx,sy}, {x,y});
        }
    }
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
        mCache.push_back({{0,0}, new Chunk('#')});
        size_t offset = Size.x * pos.y + pos.x;
        mCache.back().load("chunks.dat", offset);
        return &mCache.back();
    }
    return selected;
}

uint8_t &ChunkedMap::operator ()(const stf::Vec2d &pos)
{
    return (*(*this)[pos].mChunk)[pos];
}

ChunkRecord &ChunkedMap::operator [](const stf::Vec2d &pos)
{
    stf::Vec2d chunkBeginPos = pos / stf::Vec2d(Chunk::W, Chunk::H);
    if(pos.x < 0 || pos.y < 0 || pos.x > Size.x * Chunk::W - 1 || pos.y > Size.y * Chunk::H - 1)
        return empty;
    return *preload(chunkBeginPos);
}
