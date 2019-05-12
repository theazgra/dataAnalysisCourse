#pragma once
#include <networkLib/libs.h>

namespace azgra::networkLib
{
struct GraphComponent
{
    uint id;
    std::vector<uint> vertices;
    float modularity = 0.0f;

    GraphComponent() {}
    GraphComponent(const uint _id, const uint initialVertex) : id(_id)
    {
        vertices.push_back(initialVertex);
    }

    size_t size() const
    {
        return vertices.size();
    }
};

GraphComponent get_biggest_component(const std::vector<GraphComponent> &components);
};