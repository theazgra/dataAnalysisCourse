#pragma once
#include <networkLib/network_matrix.h>

namespace azgra::networkLib
{
class NetworkFilters
{
public:
    // For network created from vector dataset.Filter edges based on epsilon radius
    static NetworkMatrix filter_e_radius(const NetworkMatrix &src, const float radius);
    // For network created from vector dataset. Filter edges based on KNN.
    static NetworkMatrix filter_kNN(const NetworkMatrix &src, const uint k);
    // For network created from vector dataset. Filter edges based on combination of epilon radius and KNN.
    static NetworkMatrix filter_combinataion_e_knn(const NetworkMatrix &src, const float radius, const uint k);
    // Create sample from this network, with random node sampling method. Sample vertex count is equal to vertex_count() * `targetPercentSize`, which is < 1.0f.
    static NetworkMatrix filter_random_node_sampling(const NetworkMatrix &src, const float targetPercentSize);
    // Create sample from this network, with random edge sampling method. Sample vertex count is equal to vertex_count() * `targetPercentSize`, which is < 1.0f.
    static NetworkMatrix filter_random_edge_sampling(const NetworkMatrix &src, const float targetPercentSize);

    static NetworkMatrix filter_k_core(const NetworkMatrix &src, const uint k);

    static void kernighan_lin(const NetworkMatrix &src);
    static NetworkMatrix simulate_failure(const NetworkMatrix &src, const uint stepCount);
    static NetworkMatrix simulate_attack(const NetworkMatrix &src, const uint stepCount);
};

}; // namespace azgra::networkLib
