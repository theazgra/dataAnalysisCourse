#pragma once
#include <networkLib/network_matrix.h>

namespace azgra::networkLib
{
struct Community
{
    uint id;
    std::vector<uint> vertices;
};

typedef std::vector<Community> CommunityArray;
typedef std::vector<CommunityArray> CommunityEvolveArray;

class CommunityFinder
{
public:
    static CommunityArray db_scan(const float epsRadius, const uint minCommunitySize);
    static CommunityEvolveArray ravasz_hierarchical_clustering();
    static CommunityEvolveArray girvan_newman_divisive_clustering();
};
}; // namespace azgra::networkLib
