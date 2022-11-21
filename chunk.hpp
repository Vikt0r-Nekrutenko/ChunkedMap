#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "vec2d.hpp"
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

struct ICell
{
    uint8_t v;

    size_t sizeOfSelf() const
    {
        return sizeof(v);
    }

    uint8_t view() const
    {
        return v;
    }

    void save(FILE *file)
    {
        fwrite(&v, 1, 1, file);
    }

    void load(FILE *file)
    {
        fread(&v, 1, 1, file);
    }
};


class ChunkT {
public:
    ChunkT(const stf::Vec2d &size)
        : Size{size} { }

    virtual ~ChunkT()
    {
        for(size_t i = 0; i < mArray.size(); ++i)
            delete mArray.at(i);
    }

    const stf::Vec2d &size() const
    {
        return Size;
    }

    virtual ICell &operator [](const stf::Vec2d &pos)
    {
        return *mArray[Size.x * std::abs(pos.y % Size.y) + std::abs(pos.x % Size.x)];
    }

    virtual ICell &at(const stf::Vec2d &pos)
    {
        return *mArray[Size.x * std::abs(pos.y % Size.y) + std::abs(pos.x % Size.x)];
    }

    virtual size_t sizeOfSelf() const
    {
        size_t size = 0;
        for(auto &c : mArray)
            size += c->sizeOfSelf();
        return mArray.size() * size + sizeof(uint8_t);
    }

    virtual ChunkT &save(FILE *file)
    {
        for(auto &c : mArray)
            c->save(file);
        return *this;
    }

    virtual ChunkT &load(FILE *file)
    {
        for(auto &c : mArray)
            c->load(file);
        return *this;
    }

protected:

    std::vector<ICell*> mArray;
    stf::Vec2d Size;
};

struct Chunk : public ChunkT
{
    Chunk();
};

template<class ChunkT> struct ChunkRecordT
{
    stf::Vec2d mPos {0,0};
    ChunkT *mChunk {nullptr};

    ChunkRecordT<ChunkT>& load(const std::string &fileName, const size_t offset)
    {
        FILE *file = std::fopen(fileName.c_str(), "r+b");
        size_t seek = sizeof(uint8_t) + sizeof(stf::Vec2d) + mChunk->sizeOfSelf();
        std::fseek(file, offset * seek, SEEK_SET);
        uint8_t isNull = 1;
        std::fread(&isNull, sizeof(uint8_t), 1, file);

        if(isNull) {
            stf::Vec2d size = mChunk->size();
            delete mChunk;
            mChunk = new ChunkT;
            isNull = 0;

            fseek(file, -(long)sizeof(uint8_t), SEEK_CUR);
            std::fwrite(&isNull, sizeof(uint8_t), 1, file);
            std::fseek(file, sizeof(stf::Vec2d), SEEK_CUR);
            mChunk->save(file);
        }
        std::fseek(file, offset * seek + sizeof(uint8_t), SEEK_SET);
        std::fread(&mPos, sizeof(stf::Vec2d), 1, file);
        mChunk->load(file);
        std::fclose(file);
        return *this;
    }

    ChunkRecordT<ChunkT>& save(const std::string &fileName, const size_t offset)
    {
        FILE *file = std::fopen(fileName.c_str(), "r+b");
        size_t seek = sizeof(uint8_t) + sizeof(stf::Vec2d) + mChunk->sizeOfSelf();
        std::fseek(file, offset * seek + sizeof(uint8_t) + sizeof(stf::Vec2d), SEEK_SET);
        mChunk->save(file);
        std::fclose(file);
        return *this;
    }
};

#endif // CHUNK_HPP
