#pragma once
#include "libs.h"

struct GraphComponent
{
    uint id;
    std::vector<uint> vertices;

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

GraphComponent get_biggest_component(const std::vector<GraphComponent> &components)
{
    size_t maxSize = 0;
    GraphComponent max = {};
    for (const GraphComponent &component : components)
    {
        if (component.size() > maxSize)
        {
            maxSize = component.size();
            max = component;
        }
    }
    return max;
}