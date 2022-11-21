#ifndef CHUNKRECORD_HPP
#define CHUNKRECORD_HPP

#include "chunk.hpp"

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

#endif // CHUNKRECORD_HPP
