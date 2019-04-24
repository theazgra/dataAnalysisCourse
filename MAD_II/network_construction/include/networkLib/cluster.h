#pragma once
#include <networkLib/libs.h>

struct Cluster
{
    bool isDeleted = false;
    uint representative;
    std::vector<uint> vertices;

    Cluster() {}

    Cluster(uint v)
    {
        vertices.push_back(v);
        representative = v;
    }

    Cluster(const std::vector<uint> &a, const std::vector<uint> &b)
    {
        vertices.insert(vertices.begin(), a.begin(), a.end());
        vertices.insert(vertices.end(), b.begin(), b.end());

        representative = min(vertices);
    }

    bool operator==(const Cluster &b) const
    {
        return (vertices.data() == b.vertices.data());
    }
};