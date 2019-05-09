#pragma once
#include <networkLib/network_matrix.h>
#include <iomanip>
#include <sstream>

namespace azgra::networkLib
{
struct Community
{
    uint id;
    std::vector<uint> vertices;
    Community() {}
    Community(const uint v)
    {
        id = v;
        vertices.push_back(v);
    }
};

typedef std::vector<GraphComponent> CommunityArray;
typedef std::vector<std::vector<GraphComponent>> CommunityEvolveArray;

class CommunityFinder
{
public:
    // static CommunityArray db_scan(const NetworkMatrix &network, const float epsRadius, const uint minCommunitySize);
    // static CommunityEvolveArray ravasz_hierarchical_clustering(const NetworkMatrix &network, const float initialThreshold = 0.5f);

    static CommunityEvolveArray girvan_newman_divisive_clustering(const NetworkMatrix &network, std::stringstream &messageBuffer,
                                                                  const uint maxStepCount, const float targetModularity = 1.5f);

    static std::vector<uint> get_vertex_community_ids(const uint vertexCount, const CommunityArray &communities);
};
}; // namespace azgra::networkLib
