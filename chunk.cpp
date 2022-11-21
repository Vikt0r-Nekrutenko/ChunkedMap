#include "chunk.hpp"
#include <cstring>
#include <cmath>

Chunk::Chunk()
    : IChunk{{8,8}}
{
    mArray.resize(64);
    for(auto &c : mArray)
        c = new ICell{(uint8_t)('a'+rand()%('z'-'a'))};
}
