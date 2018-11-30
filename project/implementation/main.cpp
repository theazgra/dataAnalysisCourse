
#include "network_matrix.h"

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

    // auto grouped = karateClub.get_vertices_grouped_by_degree();
    // for (auto &&i : grouped)
    // {
    //     printf("Degree: %i; VC: %d\n", i.first, i.second.size());
    // }
    auto distanceMat = karateClub.get_distance_matrix();
    auto cc = karateClub.get_closeness_centrality_for_vertices(distanceMat);
    print_vector(cc);
}

void random_network_test()
{
    const uint dim = 10000;
    NetworkMatrix nm(dim, dim);
    float prob = nm.get_probability_for_symmetric_network(dim);

    printf("Vertex count: %i\n", dim);
    printf("Edge prob: %f\n", prob);
    printf("Expected average degree: %f\n", ((dim - 1) * prob));

    nm.generate_random_network(prob, false);
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
    nm.generate_scale_free_network(2, dim);

    float avgDegree = nm.get_average_degree();
    printf("Average degree: %f\n", avgDegree);

    auto distanceMatrix = nm.get_distance_matrix();
    printf("Got distance matrix\n");

    //nm.export_network("small_world.csv");
}

bool eq_mat(const NetworkMatrix &a, const NetworkMatrix &b)
{
    if (a.rows() != b.rows() || a.cols() != b.cols())
        return false;
    for (uint row = 0; row < a.rows(); row++)
    {
        for (uint col = 0; col < a.cols(); col++)
        {
            if (a.at(row, col) != b.at(row, col))
                return false;
        }
    }
    return true;
}

void dijkstra_test()
{
    NetworkMatrix nm(6, 6);
    nm.at(0, 1) = 1.0f;
    nm.at(0, 3) = 1.0f;
    nm.at(0, 4) = 1.0f;

    nm.at(1, 0) = 1.0f;
    nm.at(1, 2) = 1.0f;
    nm.at(1, 3) = 1.0f;

    nm.at(2, 1) = 1.0f;
    nm.at(2, 3) = 1.0f;
    nm.at(2, 5) = 1.0f;

    nm.at(3, 0) = 1.0f;
    nm.at(3, 1) = 1.0f;
    nm.at(3, 2) = 1.0f;
    nm.at(3, 4) = 1.0f;

    nm.at(4, 0) = 1.0f;
    nm.at(4, 5) = 1.0f;

    nm.at(5, 4) = 1.0f;
    nm.at(5, 2) = 1.0f;

    auto bfs = nm.get_distance_matrix(false);
    auto dij = nm.get_distance_matrix(true);

    assert(eq_mat(bfs, dij) && "NOT EQUAL");

    dij.print_matrix();
}
// Network is always indexed from zero!!! If in CSV file there is not vertex 0, then vertex 1 is renamed to 0.
int main(int argc, char **argv)
{
    /*
    1. Implementace jednoho z algoritmů generování grafu na základě modelů probíraných na přednáškách nebo jiného z této oblasti (kromě Erdös - Renyi). 
        
        add 1  . Výsledkem aplikace algoritmu bude textový soubor osahující seznam hran (dvojic vrcholů). 
    
    2. Implementace některého z algoritmů analyzujícího vlastnosti grafu (distribuce stupňů vrcholů grafu a průměrný stupeň, 
        shlukovací koeficient vrcholu a průměrný shlukovací koeficient grafu, průměr grafu apod.). 
        
        add 2. Algoritmus musí umět zpracovat graf nejméně s tisíci vrcholy a řádově s tisíci hranami. 
    
    3. Výsledkem aplikace algoritmu bude textový soubor obsahující shrnutí získaných informací. 

    4. Graf pro testování algoritmu může být umělý (např. generovaný vlastním algoritmem z bodu 1), referenční (z webových zdrojů nebo z používaných 
        nástrojů), nebo vlastní získaný z jiného zdroje. Podle zvolené úlohy se může jednat jak o neorientovaný, tak o orientovaný graf, jak o nevážený
        (neohodnocený), tak o vážený (ohodnocený) graf. 
        
        add 4. Výstupy algoritmu budou validovány použitím některého z nástrojů (např. R), jehož výstupy by měly odpovídat výstupům algoritmu. 
*/
    char *csvFile = "/home/mor0146/github/dataAnalysisCourse/data/KarateClub.csv";
#if 0
    auto karateEdges = load_edge_pairs(csvFile, ";");
    uint vc = get_vertex_count_from_edge_pairs(karateEdges);
    NetworkMatrix network = NetworkMatrix(vc, vc);
    network.load_from_edges(karateEdges);
    network.complete_analysis("Zachary-karate-club", "../out/zacharyReport.txt");

#else
    // PROJECT
    const uint vertexCount = 1000;
    const uint m = 2;
    const char *exportNetworkFile = "../out/scaleFreeNet.csv";
    const char *reportFile = "../out/scaleFreeReport.txt";

    // 1. Scale free network generation, exporting to exportNetworkFile.
    NetworkMatrix network(vertexCount, vertexCount);
    // We are starting with 3 vertices.
    network.generate_scale_free_network(m, (vertexCount - 3));
    network.export_network(exportNetworkFile);

    // 2. Network Analysis
    network.complete_analysis("ScaleFreeNetwork", reportFile);
#endif
    return 0;
}
