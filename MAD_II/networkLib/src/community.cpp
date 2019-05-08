#include <networkLib/community.h>
namespace azgra::networkLib
{
constexpr int HeavysideDelta(const float input) { return input > 0 ? 1 : 0; }

CommunityArray CommunityFinder::db_scan(const NetworkMatrix &src, const float epsRadius, const uint minCommunitySize)
{
    // For vertex check if he has in epsRadius atleast minCommunitySize neighbors
    //  True - start community
    //  False - mark as noise, skip vertex, may be found later by different community
    // Communities will be Overlapping, probably very dependent on parameters.
}

CommunityEvolveArray CommunityFinder::ravasz_hierarchical_clustering(const NetworkMatrix &src, const float initialThreshold)
{
    NetworkMatrix network(src);
    CommunityEvolveArray result;
    CommunityArray communities;

    // Assign each node to a community of its own and evaluate x_ij for all node pairs.
    for (size_t v = 0; v < network.vertex_count(); v++)
    {
        communities.push_back(Community(v));
    }

    //  x_ij = J(i,j) / min(k_i,k_j)+1-heaviside_delta(A_ij)
    //      J(i,j) number of common neighbors or i and j, add 1 if there is direct edge between i and j
    //      heaviside_delta A_ij > 0 ? 1 : 0;
    //      k_i,k_j, i and j degrees
    NetworkMatrix similarityMatrix(network.rows(), network.cols());

    for (size_t u = 0; u < network.rows(); u++)
    {
        for (size_t v = u + 1; v < network.cols(); v++)
        {
            auto uNeighborhood = network.get_neighbors(u);
            auto vNeighborhood = network.get_neighbors(v);

            uint sharedNeighborsCount = network.get_count_of_same_neighbors(uNeighborhood, vNeighborhood);
            float J = sharedNeighborsCount + (network.is_edge_at(u, v) ? 1 : 0);
            uint minDeg = min_from(uNeighborhood.size(), vNeighborhood.size());
            float delta = HeavysideDelta(network.at(u, v));

            float similarity = J / (minDeg + 1 - delta);
            similarityMatrix.at(u, v) = similarity;
            similarityMatrix.at(v, u) = similarity;
        }
    }

    

    // Find the community pair or the node pair with the highest similarity and merge them into a single community.
    //  similarity measured by single/complete/average linkage
    // Calculate the similarity between the new community and all other communities.
    // Repeat Steps 2 and 3 until all nodes form a single community.
}

CommunityEvolveArray CommunityFinder::girvan_newman_divisive_clustering(const NetworkMatrix &src)
{

    // Compute the centrality x_ij of each link.
    //  x_ij is proportional to the number of shortest paths between all node pairs that run along the link (i,j).
    // Remove the link with the largest centrality. In case of a tie, choose one link randomly.
    // Recalculate the centrality of each link for the altered network.
    // Repeat steps 2 and 3 until all links are removed.
}

}; // namespace azgra::networkLib