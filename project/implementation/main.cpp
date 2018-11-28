
// #include <iostream>
// #include <random>
// #include <map>
// #include <stdio.h>
#include "network_matrix.h"
//#include "network_matrix.cpp"

// Network is always indexed from zero!!! If in CSV file there is not vertex 0, then vertex 1 is renamed to 0.
int main(int, char **)
{
    auto karateEdges = load_edge_pairs("../../../data/KarateClub.csv", ";");
    uint vertexCount = get_vertex_count_from_edge_pairs(karateEdges);

    NetworkMatrix karateClub = NetworkMatrix(vertexCount, vertexCount);
    karateClub.load_from_edges(karateEdges);
    //karateClub.print_matrix();

    float avgDegree = karateClub.get_average_degree();
    float avgClustCoeff = karateClub.get_average_clustering_coefficient();

    // printf("Avg. degree: %f\n", avgDegree);
    // printf("Avg. clustering coeff: %f\n", avgClustCoeff);
    // std::vector<float> coeffs = karateClub.get_clustering_coeff_for_all_vertices();
    // uint index = 0;
    // for (const float &f : coeffs)
    //     printf("%i:%f\n", index++, f);

    auto grouped = karateClub.get_vertices_grouped_by_degree();

    for (auto &&i : grouped)
    {
        printf("Degree: %i; VC: %i\n", i.first, i.second.size());
    }
    return 0;
    /*


    std::random_device rd;
    std::mt19937 gen(rd());
    double weights[] = {0.2, 0.2, 0.6};

    std::discrete_distribution<int> d = std::discrete_distribution<int>(std::begin(weights), std::end(weights));

    for (size_t i = 0; i < 100; i++)
    {
        std::cout << d(gen) << std::endl;
    }
*/
    // std::map<int, int> m;
    // for (int n = 0; n < 10000; ++n)
    // {
    //     ++m[d(gen)];
    // }
    // for (auto p : m)
    // {
    //     std::cout << p.first << " generated " << p.second << " times\n";
    // }
    return 0;
}
