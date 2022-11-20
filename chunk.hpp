#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "renderer.hpp"

struct Chunk
{
    static constexpr int W = 3;
    static constexpr int H = W;
    static constexpr int L = W * H;

    uint8_t mArray[L];
    uint8_t sym = 0;

    Chunk();
    Chunk(uint8_t s);

    void show(stf::Renderer &renderer, const stf::Vec2d &posOnScreen, const stf::Vec2d& posInArray) const;
    uint8_t& operator [](const stf::Vec2d &pos);
};

#endif // CHUNK_HPP
