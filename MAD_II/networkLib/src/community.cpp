#include <networkLib/community.h>
namespace azgra::networkLib
{
CommunityArray CommunityFinder::db_scan(const float epsRadius, const uint minCommunitySize)
{
    // For vertex check if he has in epsRadius atleast minCommunitySize neighbors
    //  True - start community
    //  False - mark as noise, skip vertex, may be found later by different community
    // Communities will be Overlapping, probably very dependent on parameters.
}

CommunityEvolveArray CommunityFinder::ravasz_hierarchical_clustering()
{

    // Assign each node to a community of its own and evaluate x_ij for all node pairs.
    //  x_ij = J(i,j) / min(k_i,k_j)+1-heaviside_delta(A_ij)
    //      J(i,j) number of common neighbors or i and j, add 1 if there is direct edge between i and j
    //      heaviside_delta A_ij > 0 ? 1 : 0;
    //      k_i,k_j, i and j degrees
    // Find the community pair or the node pair with the highest similarity and merge them into a single community.
    //  similarity measured by single/complete/average linkage
    // Calculate the similarity between the new community and all other communities.
    // Repeat Steps 2 and 3 until all nodes form a single community.
}
CommunityEvolveArray CommunityFinder::girvan_newman_divisive_clustering()
{

    // Compute the centrality x_ij of each link.
    //  x_ij is proportional to the number of shortest paths between all node pairs that run along the link (i,j).
    // Remove the link with the largest centrality. In case of a tie, choose one link randomly.
    // Recalculate the centrality of each link for the altered network.
    // Repeat steps 2 and 3 until all links are removed.
}
}; // namespace azgra::networkLib