#include "chunk.hpp"
#include <cstring>
#include <cmath>

Chunk::Chunk()
    : ChunkT{{8,8}}
{
    mArray.resize(64, {(uint8_t)('a'+rand()%('z'-'a'))});
}
