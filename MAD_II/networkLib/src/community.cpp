#include <networkLib/community.h>
namespace azgra::networkLib
{
constexpr int HeavysideDelta(const float input) { return input > 0 ? 1 : 0; }

// CommunityArray CommunityFinder::db_scan(const NetworkMatrix &src, const float epsRadius, const uint minCommunitySize)
// {
//     // For vertex check if he has in epsRadius atleast minCommunitySize neighbors
//     //  True - start community
//     //  False - mark as noise, skip vertex, may be found later by different community
//     // Communities will be Overlapping, probably very dependent on parameters.
// }

// CommunityEvolveArray CommunityFinder::ravasz_hierarchical_clustering(const NetworkMatrix &src, const float initialThreshold)
// {
//     NetworkMatrix network(src);
//     CommunityEvolveArray result;
//     CommunityArray communities;

//     // Assign each node to a community of its own and evaluate x_ij for all node pairs.
//     for (size_t v = 0; v < network.vertex_count(); v++)
//     {
//         communities.push_back(Community(v));
//     }

//     //  x_ij = J(i,j) / min(k_i,k_j)+1-heaviside_delta(A_ij)
//     //      J(i,j) number of common neighbors or i and j, add 1 if there is direct edge between i and j
//     //      heaviside_delta A_ij > 0 ? 1 : 0;
//     //      k_i,k_j, i and j degrees
//     NetworkMatrix similarityMatrix(network.rows(), network.cols());

//     for (size_t u = 0; u < network.rows(); u++)
//     {
//         for (size_t v = u + 1; v < network.cols(); v++)
//         {
//             auto uNeighborhood = network.get_neighbors(u);
//             auto vNeighborhood = network.get_neighbors(v);

//             uint sharedNeighborsCount = network.get_count_of_same_neighbors(uNeighborhood, vNeighborhood);
//             float J = sharedNeighborsCount + (network.is_edge_at(u, v) ? 1 : 0);
//             uint minDeg = min_from(uNeighborhood.size(), vNeighborhood.size());
//             float delta = HeavysideDelta(network.at(u, v));

//             float similarity = J / (minDeg + 1 - delta);
//             similarityMatrix.at(u, v) = similarity;
//             similarityMatrix.at(v, u) = similarity;
//         }
//     }
//     return result;

//     // Find the community pair or the node pair with the highest similarity and merge them into a single community.
//     //  similarity measured by single/complete/average linkage
//     // Calculate the similarity between the new community and all other communities.
//     // Repeat Steps 2 and 3 until all nodes form a single community.
// }

CommunityEvolveArray CommunityFinder::girvan_newman_divisive_clustering(const NetworkMatrix &src, std::stringstream &messageBuffer,
                                                                        const uint maxStepCount, const float targetModularity)
{
    messageBuffer << "Invoked: CommunityFinder::girvan_newman_divisive_clustering ..." << std::endl;

    NetworkMatrix workCopy(src);
    std::random_device rd;
    std::mt19937 mt(rd());

    CommunityEvolveArray result;

    // Recalculate the centrality of each link for the altered network.
    // Repeat steps 2 and 3 until all links are removed.
    for (size_t step = 0; step < maxStepCount; step++)
    {
        // Compute the centrality x_ij of each link.
        //  x_ij is proportional to the number of shortest paths between all node pairs that run along the link (i,j).
        NetworkMatrix ebMat = workCopy.get_edge_betweenness_matrix();

        // Remove the link with the largest centrality. In case of a tie, choose one link randomly.
        std::vector<UndirectedEdge> edgesToRemove;
        float maxBc = 0.0f;
        for (uint row = 0; row < ebMat.rows(); row++)
        {
            for (uint col = row + 1; col < ebMat.cols(); col++)
            {
                if (ebMat.at(row, col) > maxBc)
                {
                    maxBc = ebMat.at(row, col);
                    edgesToRemove.clear();
                    edgesToRemove.push_back(UndirectedEdge(row, col));
                }
                else if (ebMat.at(row, col) == maxBc)
                {
                    edgesToRemove.push_back(UndirectedEdge(row, col));
                }
            }
        }

        if (edgesToRemove.size() == 0)
        {
            messageBuffer << "No more edges to remove" << std::endl;
            fprintf(stdout, "No more edges to remove\n");
            break;
        }

        UndirectedEdge removedEdge;
        if (edgesToRemove.size() == 1)
        {
            removedEdge = edgesToRemove[0];
        }
        else
        {
            std::uniform_int_distribution<int> dist(0, edgesToRemove.size() - 1);
            removedEdge = edgesToRemove[dist(rd)];
        }
        workCopy.at(removedEdge.u, removedEdge.v) = 0.0f;
        workCopy.at(removedEdge.v, removedEdge.u) = 0.0f;

        auto communities = workCopy.get_components();
        float networkModularity = 0.0f;
        // Calculate the modularity for all components and for whole network.
        uint totalEdgeCount = workCopy.edge_count();
        bool moreThanOneCommunity = communities.size() > 1;
        for (size_t communityId = 0; communityId < communities.size(); communityId++)
        {
            uint communityEdgeCount = moreThanOneCommunity ? workCopy.get_edge_count_in_component(communities[communityId].vertices) : totalEdgeCount;
            uint communityDegree = workCopy.get_total_degree_of_community(communities[communityId].vertices);

            float cModularity = (static_cast<float>(communityEdgeCount) / static_cast<float>(totalEdgeCount)) -
                                pow(static_cast<float>(communityDegree) / static_cast<float>(2 * totalEdgeCount), 2);

            communities[communityId].modularity = cModularity;
            networkModularity += cModularity;
        }

        result.push_back(communities);
        fprintf(stdout, "Finished iteration %u, # of communities: %lu, EC: %u, Network modularity: %.4f\n",
                static_cast<uint>(step + 1), communities.size(), totalEdgeCount, networkModularity);

        messageBuffer << std::setprecision(4);
        messageBuffer << "Finished iteration " << static_cast<uint>(step + 1)
                      << ", # of communities: " << communities.size()
                      << ", EC: " << totalEdgeCount
                      << ", Network modularity: " << networkModularity
                      << std::endl;

        if (networkModularity >= targetModularity)
        {
            messageBuffer << "Target modularity was reached." << std::endl;
            fprintf(stdout, "Target modularity was reached.\n");
            break;
        }
    }
    messageBuffer << "Finished Girvan-Newman clustering." << std::endl;
    return result;
}

std::vector<uint> CommunityFinder::get_vertex_community_ids(const uint vertexCount, const CommunityArray &communities)
{
    std::vector<uint> result(vertexCount);

    for (const GraphComponent &community : communities)
    {
        for (const uint &communityVertex : community.vertices)
        {
            result[communityVertex] = community.id;
        }
    }

    return result;
}

}; // namespace azgra::networkLib