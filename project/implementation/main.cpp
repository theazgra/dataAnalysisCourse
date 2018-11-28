
// #include <iostream>
// #include <random>
// #include <map>
// #include <stdio.h>
#include "network_matrix.h"
//#include "network_matrix.cpp"

void karate_test()
{
    auto karateEdges = load_edge_pairs("../../../data/KarateClub.csv", ";");
    uint vertexCount = get_vertex_count_from_edge_pairs(karateEdges);

    NetworkMatrix karateClub = NetworkMatrix(vertexCount, vertexCount);
    karateClub.load_from_edges(karateEdges);
    //karateClub.print_matrix();

    float avgDegree = karateClub.get_average_degree();
    float avgClustCoeff = karateClub.get_average_clustering_coefficient();

    printf("Avg. degree: %f\n", avgDegree);
    printf("Avg. clustering coeff: %f\n", avgClustCoeff);
    std::vector<float> coeffs = karateClub.get_clustering_coeff_for_all_vertices();
    uint index = 0;
    for (const float &f : coeffs)
        printf("%i:%f\n", index++, f);

    auto grouped = karateClub.get_vertices_grouped_by_degree();

    for (auto &&i : grouped)
    {
        printf("Degree: %i; VC: %d\n", i.first, i.second.size());
    }
}

void random_network_test()
{
    const uint dim = 10000;
    NetworkMatrix nm(dim, dim);
    float prob = nm.get_probability_for_symmetric_network(dim);

    printf("Vertex count: %i\n", dim);
    printf("Edge prob: %f\n", prob);
    printf("Expected average degree: %f\n", ((dim - 1) * prob));

    nm.generate_random_network(dim, prob, false);
    printf("Done generating.\n");

    float avgDegree = nm.get_average_degree();
    float avgClustCoeff = nm.get_average_clustering_coefficient();

    printf("Average degree: %f\n", avgDegree);
    printf("Average clustering coeff: %f\n", avgClustCoeff);
}

void scale_free_network_test()
{
    const uint dim = 1000;
    NetworkMatrix nm(dim, dim);
    nm.generate_scale_free_network(2, 2500);

    float avgDegree = nm.get_average_degree();
    printf("Average degree: %f\n", avgDegree);

    nm.export_network("small_world.csv");
}

// Network is always indexed from zero!!! If in CSV file there is not vertex 0, then vertex 1 is renamed to 0.
int main(int, char **)
{
    //karate_test();
    //random_network_test();
    //scale_free_network_test();

    NetworkMatrix nm(3, 3);
    nm.at(0, 0) = 1.0;
    nm.at(0, 1) = 2.0;
    nm.at(0, 2) = 3.0;

    nm.at(1, 0) = 0.0;
    nm.at(1, 1) = 0.0;
    nm.at(1, 2) = 0.0;

    nm.at(2, 0) = 1.0;
    nm.at(2, 1) = 2.0;
    nm.at(2, 2) = 3.0;

    nm.print_matrix();
    nm.transpose();
    printf("==============================\n");
    nm.print_matrix();
    return 0;
}
