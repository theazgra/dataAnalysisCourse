#pragma once

namespace azgra::networkLib
{
typedef unsigned int uint;

struct UndirectedEdge
{
    uint u;
    uint v;
    float betweenness = 0.0f;
    UndirectedEdge() {}
    UndirectedEdge(uint _u, uint _v) : u(_u), v(_v) {}
    UndirectedEdge(uint _u, uint _v, float _b) : u(_u), v(_v), betweenness(_b) {}

    bool operator==(const UndirectedEdge &other) const
    {
        return ((u == other.u && v == other.v) || (u == other.v && v == other.u));
    }
};
}; // namespace azgra::networkLib