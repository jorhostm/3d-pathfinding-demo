#pragma once
#include <vector>
struct VectorXYZ
{
    int x, y, z;
    VectorXYZ(int x_, int y_, int z_) : x(x_), y(y_), z(z_)
    {
    }
    bool operator==(const VectorXYZ& v) const
    {
        return v.x == x && v.y == y && v.z == z;
    }
};

struct VectorHash
{
    std::size_t operator()(VectorXYZ const& v) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(v.x);
        std::size_t h2 = std::hash<int>{}(v.y);
        std::size_t h3 = std::hash<int>{}(v.z);
        return h1 ^ h2 ^ h3;
    }
};