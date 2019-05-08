#pragma once
#include <networkLib/network_matrix.h>

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

typedef std::vector<Community> CommunityArray;
typedef std::vector<CommunityArray> CommunityEvolveArray;

class CommunityFinder
{
public:
    static CommunityArray db_scan(const NetworkMatrix &network, const float epsRadius, const uint minCommunitySize);
    static CommunityEvolveArray ravasz_hierarchical_clustering(const NetworkMatrix &network, const float initialThreshold = 0.5f);
    static CommunityEvolveArray girvan_newman_divisive_clustering(const NetworkMatrix &network);
};
}; // namespace azgra::networkLib
