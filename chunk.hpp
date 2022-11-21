#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "vec2d.hpp"
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

struct ICell
{
    virtual ~ICell() = default;
    virtual size_t sizeOfSelf() const = 0;
    virtual void save(FILE *file) = 0;
    virtual void load(FILE *file) = 0;
};

class IChunk {
public:
    IChunk(const stf::Vec2d &size);

    virtual ~IChunk();
    virtual size_t sizeOfSelf() const;

    virtual ICell* operator [](const stf::Vec2d &pos);
    virtual ICell* put(const stf::Vec2d &pos, ICell *cell);
    virtual ICell* at(const stf::Vec2d &pos);

    virtual IChunk &save(FILE *file);
    virtual IChunk &load(FILE *file);

    const stf::Vec2d &size() const;

protected:

    std::vector<ICell*> mArray;
    stf::Vec2d Size;
};

#endif // CHUNK_HPP
