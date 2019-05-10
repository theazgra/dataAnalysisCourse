// #include "external/args.hxx"
// #include "network_matrix.h"
// #include "multilayer_network.h"
#include <networkLib/network_generator.h>
#include <networkLib/Stopwatch.h>
#include <networkLib/multilayer_network.h>
#include <networkLib/community.h>

using namespace azgra::networkLib;
int main(int argc, char **argv)
{
    // auto fbNet = NetworkMatrix("/home/mor0146/github/dataAnalysisCourse/data/project/facebook_combined.csv", " ", true);
    // fprintf(stdout, "Imported, VC: %u, EC: %u\n", fbNet.vertex_count(), fbNet.edge_count());

    auto collabNet = NetworkMatrix("/home/mor0146/github/dataAnalysisCourse/data/project/gr_qc_collab.csv", "\t", true);
    fprintf(stdout, "Imported, VC: %u, EC: %u\n", collabNet.vertex_count(), collabNet.edge_count());

    //auto network = NetworkMatrix("/home/mor0146/Desktop/ba50.csv");
    //network.get_edge_betweenness_matrix().export_edge_betweenness("/home/mor0146/Desktop/EB_ba50.csv");
    //auto communityEvolve = CommunityFinder::girvan_newman_divisive_clustering(network, 200);

    // int hour = 3600;
    // //auto ml = MultiLayerNetwork<std::string>::import_from_mpx("/home/mor0146/github/dataAnalysisCourse/data/florentine.mpx");
    // auto ml = MultiLayerNetwork<uint>::import_from_temporal_data("/home/mor0146/github/dataAnalysisCourse/data/ht09_contact_list.dat", '\t', hour * 10);

    // std::vector<uint> layers = {0, 2, 4}; //ml.get_all_layers_ids();
    // auto neighCent = ml.get_actors_neighborhood_centrality(layers);
    // auto neighCentEx = ml.get_actors_exclusive_neighborhood_centrality(layers);
    // auto actorDeg = ml.get_actors_degree_centrality(layers);
    // auto red = ml.get_actors_connective_redundancy(layers);
    // for (auto &&actorCent : neighCent)
    // {
    //     fprintf(stdout, "Actor: %-5u, ncent: %5u, xncent.: %5u Degree cent: %5u, Conn redundancy: %5.4f\n", actorCent.first,
    //             actorCent.second, neighCentEx[actorCent.first], actorDeg[actorCent.first], red[actorCent.first]);
    // }

    return 0;
    // if (rounded != 2.991f)
    //     throw new std::logic_error("Broke the calculation");

    /*
    NetworkMatrix baNetwork = NetworkMatrix(500, 500);
    baNetwork.generate_scale_free_network(10, 500, 3);
    NetworkMatrix ba1(baNetwork);
    NetworkMatrix ba2(baNetwork);
    analysis(ba1, "/home/mor0146/github/dataAnalysisCourse/data/failureAndAttack/ba_fail.csv", false);
    analysis(ba2, "/home/mor0146/github/dataAnalysisCourse/data/failureAndAttack/ba_attack.csv", true);
    */
    /*
    NetworkMatrix randomNetwork = NetworkMatrix(500, 500);
    float randProb = 1480.0f / ((500.0f * 499.0f) / 2.0f);
    randomNetwork.generate_random_network(randProb, false);
    NetworkMatrix rand1(randomNetwork);
    NetworkMatrix rand2(randomNetwork);
    analysis(rand1, "/home/mor0146/github/dataAnalysisCourse/data/failureAndAttack/random_fail.csv", false);
    analysis(rand2, "/home/mor0146/github/dataAnalysisCourse/data/failureAndAttack/random_attack.csv", true);
    */
    /*
    NetworkMatrix airportNetwork = NetworkMatrix("/home/mor0146/github/dataAnalysisCourse/data/USairport500.csv", -1);
    NetworkMatrix airport1(airportNetwork);
    NetworkMatrix airport2(airportNetwork);
    analysis(airport1, "/home/mor0146/github/dataAnalysisCourse/data/failureAndAttack/usairport_fail.csv", false);
    analysis(airport2, "/home/mor0146/github/dataAnalysisCourse/data/failureAndAttack/usairport_attack.csv", true);
    */
    /*
    args::ArgumentParser parser("Network analysis. - MOR0146", "Required arguments are in < > optional in [].");
    args::Group methodGroup(parser, "Methods", args::Group::Validators::AtMostOne);
    args::Group modelGroup(parser, "Network models", args::Group::Validators::AtMostOne);

    args::Group networkFromVectorDataMethod(parser, "Network from vector data construction methods", args::Group::Validators::AtMostOne);
    args::HelpFlag _help(methodGroup, "help", "Print help", {'h', "help"});

    args::Flag _import(methodGroup, "import", "Import network from csv file (';' as delimiter) and run analysis. <file> <report-file>", {'i', "import"});
    args::Flag _generate(methodGroup, "generate", "Generate and save network. <vertex-count> <network-file> <report-file> [probability]",
                         {'g', "generate"});

    args::Group epidemicModelGroup(parser, "Epidemic models", args::Group::Validators::AtMostOne);
    args::Flag _epidemicMethod(methodGroup, "Epidemic", "Simulate chosen epidemic model <epidemic-model> <report-folder>", {"epidemic"});

    args::Flag _constructFromVectorData(methodGroup, "construct-from-vector-data", "Construct network from vector data. <file> [ConstructionMethod] <report-file>", {"construct-from-vector-data"});
    args::Flag _sampling(methodGroup, "Network sampling", "Do sampling homework.", {"sampling"});
    args::Flag _kernighanLin(methodGroup, "Kernighan-Lin algorithm", "Kernighan-lin clustering. <file>", {"kernighan-lin"});
    args::Flag _hierarchicalClustering(methodGroup, "Hierarchical clustering", "Hierarchical clustering use <v> as cluster count. <file> <report-file> <v>", {"hierarchical-clustering"});
    args::Flag _kCore(methodGroup, "K-Core", "K-Core use <v> as k parameter. <file> <report-file> <v>", {"k-core"});

    args::Flag _demo(methodGroup, "demo", "Generate and analyse Network model generated by Barabasi-Albert model.", {'d', "demo"});

    args::ValueFlag<std::string> _file(parser, "file", "CSV file.", {'f', "file"});
    args::ValueFlag<std::string> _reportFile(parser, "report-file", "Report text file.", {'r', "report"});
    args::ValueFlag<uint> _vertexCount(parser, "vertex-count", "Vertex count.", {'v', "vertex-count"});
    args::ValueFlag<float> _prob(parser, "probability.", "Edge probability.", {'p', "edge-probability"});
    args::ValueFlag<uint> _edgeCountInStep(parser, "m", "Number of edges to add in one step", {'m'});

    args::Flag _baMethod(modelGroup, "BA", "Use Barabasi-Albert model for generating..", {"ba"});
    args::Flag _holmeKimMethod(modelGroup, "Holme-Kim", "Use Holme-Kim model for generating..", {"holme-kim"});
    args::Flag _bianconiMethod(modelGroup, "Bianconi", "Use Bianconi model for generating..", {"bianconi"});
    args::Flag _linkSelectionMethod(modelGroup, "LinkSelection", "Use Link Selection model for generating.", {"link-selection"});
    args::Flag _copyMethod(modelGroup, "CopyModel", "Use Copy model for generating..", {"copy"});

    args::Flag _epsConstructionMethod(networkFromVectorDataMethod, "Epsilon radius", "Use epsilon as threshold value.", {"eps"});
    args::Flag _kNNConstructionMethod(networkFromVectorDataMethod, "k-NN", "Find k nearest neighbors.", {"knn"});
    args::Flag _combinedConstructionMethod(networkFromVectorDataMethod, "Combined", "Combine epsilon radius with k-NN", {"eps-knn"});

    args::Flag _siModel(epidemicModelGroup, "SI model", "SI model", {"SI"});
    args::Flag _sisModel(epidemicModelGroup, "SIS model", "SIS model", {"SIS"});
    args::Flag _sirModel(epidemicModelGroup, "SIR model", "SIR model", {"SIR"});
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help &he)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError &e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
*/
    /*
    if (_epidemicMethod)
    {
        NetworkMatrix baNetwork = NetworkMatrix(200, 200);
        baNetwork.generate_scale_free_network(10, 200, 3);
        baNetwork.export_network("epidemic_model.csv");
        printf("Epidemic simulation on barabasi-albert model\n");

        std::vector<EpidemicIterationInfo> epidemicResult;
        if (_siModel)
            epidemicResult = SI_epidemic_model(baNetwork, 5, 0.5f, 10);
        else if (_sisModel)
            epidemicResult = SIS_epidemic_model(baNetwork, 5, 0.5f, 2, 10);
        else if (_sirModel)
            epidemicResult = SIR_epidemic_model(baNetwork, 5, 0.5f, 3, 10);
        else
        {
            printf("No epidemic model was chosen.\n");
            return 1;
        }

        print_epidemic_stats(epidemicResult);
        if (_reportFile.Matched())
            save_epidemic_stats(epidemicResult, _reportFile.get_actors().c_str());
        return 0;
    }

    if (_kCore)
    {
        if (!_file.Matched() || !_reportFile.Matched() || !_vertexCount.Matched())
        {
            printf("Network file, report file and vertex count as k paramtere are required!\n");
            return 1;
        }

        NetworkMatrix karataNet = NetworkMatrix(_file.get_actors().c_str());
        karataNet.filter_k_core(_vertexCount.get_actors());
        karataNet.export_network(_reportFile.get_actors().c_str());

        return 0;
    }

    if (_hierarchicalClustering)
    {
        if (!_file.Matched() || !_reportFile.Matched() || !_vertexCount.Matched())
        {
            printf("Network file, report file and vertex count as cluster count are required!\n");
            return 1;
        }

        NetworkMatrix karataNet = NetworkMatrix(_file.get_actors().c_str());
        karataNet.hierarchical_clustering(_vertexCount.get_actors(), _reportFile.get_actors().c_str(), Linkaget_actorsype_Average);

        return 0;
    }

    if (_kernighanLin)
    {
        if (!_file.Matched())
        {
            printf("Network file is required!\n");
            return 1;
        }

        auto karateNetwork = NetworkMatrix(_file.get_actors().c_str());
        printf("VC: %5u; EC: %5u\n", karateNetwork.vertex_count(), karateNetwork.edge_count());
        karateNetwork.kernighan_lin();

        return 0;
    }

    if (_sampling)
    {
        uint vertexCount = 1000;
        NetworkMatrix baModel = NetworkMatrix(1000, 1000);
        printf("Generating BA model...\n");
        baModel.generate_scale_free_network(3, 2, vertexCount - 3);
        baModel.export_network("output/ba.csv");
        printf("Saved BA model. VC: %5u EC: %5u\n", baModel.vertex_count(), baModel.edge_count());

        NetworkMatrix baRNS = baModel.filter_random_node_sampling(0.15f);
        NetworkMatrix baRES = baModel.filter_random_edge_sampling(0.15f);

        baRNS.export_network("output/baRNS.csv");
        baRES.export_network("output/baRES.csv");

        float prob = 1997.0f / ((1000.0f * 999.0f) / 2.0f);
        //printf("Probability: %f\n", prob);
        NetworkMatrix randomModel = NetworkMatrix(1000, 1000);
        printf("Generating random model...\n");
        randomModel.generate_random_network(prob, false);
        randomModel.export_network("output/random.csv");
        printf("Saved random model. VC: %5u EC: %5u\n", randomModel.vertex_count(), randomModel.edge_count());

        NetworkMatrix randomModelRNS = randomModel.filter_random_node_sampling(0.15f);
        NetworkMatrix randomModelRES = randomModel.filter_random_edge_sampling(0.15f);

        randomModelRNS.export_network("output/randomRNS.csv");
        randomModelRES.export_network("output/randomRES.csv");
        return 0;
    }

    if (_constructFromVectorData)
    {
        auto irisDataset = read_iris_file(_file.get_actors().c_str());
        NetworkMatrix irisMat = NetworkMatrix(irisDataset);

        if (_epsConstructionMethod)
        {
            printf("Chosen method of epsilon radius.\n");
            irisMat.filter_e_radius(0.75);
        }
        else if (_kNNConstructionMethod)
        {
            printf("Chosen method kNN.\n");
            irisMat.filter_kNN(5);
        }
        else if (_combinedConstructionMethod)
        {
            printf("Chosen combined method of epsilon radius and kNN.\n");
            irisMat.filter_combinataion_e_knn(0.75, 5);
        }
        else
        {
            printf("No construction method was chosen.\n");
            return 0;
        }

        const char *fName = (bool)_reportFile ? _reportFile.get_actors().c_str() : "unnamed-report.csv";

        irisMat.export_network(fName);
        printf("Finished.\n");
        return 0;
    }

    if (_import)
    {
        int offset;
        auto loadedEdges = load_edge_pairs(_file.get_actors().c_str(), ";", offset);
        uint vc = get_actors_vertex_count_from_edge_pairs(loadedEdges);
        NetworkMatrix network = NetworkMatrix(vc, vc);
        network.load_from_edges(loadedEdges);
        printf("Loaded network from %s.\n", _file.get_actors().c_str());
        network.complete_analysis("Imported", _reportFile.get_actors().c_str());
        printf("Completed analysis in %s\n", _reportFile.get_actors().c_str());
        return 0;
    }

    if (_generate)
    {
        printf("Chosen to generate network with final vertex count: %u\n", _vertexCount.get_actors());
        if (!_vertexCount.Matched())
        {
            printf("Vertex count wasn't specified\n");
            return 1;
        }
        uint vertexCount = _vertexCount.get_actors();
        NetworkMatrix nm(vertexCount, vertexCount);

        if (_holmeKimMethod)
        {
            printf("Holme-Kim method with probability %f edge count: %u\n", _prob.get_actors(), _edgeCountInStep.get_actors());
            nm.generate_holme_kim(_prob.get_actors(), _edgeCountInStep.get_actors());
            nm.export_network(_file.get_actors().c_str());
            printf("Generated and saved in %s\n", _file.get_actors().c_str());
            return 0;
        }
        else if (_bianconiMethod)
        {
            if (!_prob.Matched() || !_edgeCountInStep.Matched() || !_file.Matched())
            {
                printf("Bianconi model need probability (p), edge count (m) and file (f).\n");
                return 1;
            }
            printf("Bianconi method with probability %f edge count: %u\n", _prob.get_actors(), _edgeCountInStep.get_actors());
            nm.generate_bianconi(_prob.get_actors(), _edgeCountInStep.get_actors());
            nm.export_network(_file.get_actors().c_str());
            printf("Generated and saved in %s\n", _file.get_actors().c_str());
            return 0;
        }
        else if (_baMethod)
        {
            printf("M IS %i\n", _edgeCountInStep.get_actors());

            nm.generate_scale_free_network((_edgeCountInStep ? _edgeCountInStep.get_actors() : 2), vertexCount - 3);
            printf("Generated network using Barabasi-Albert method.\n");
            nm.export_network(_file.get_actors().c_str());
            printf("Saved network in: %s\n", _file.get_actors().c_str());
            nm.complete_analysis("Generated using Barabasi-Albert method.", _reportFile.get_actors().c_str(), true, true);
            printf("Completed analysis in %s\n", _reportFile.get_actors().c_str());
            return 0;
        }
        else if (_linkSelectionMethod || _copyMethod)
        {

            if (!_file.Matched())
            {
                printf("File (f) is required\n");
                return 1;
            }
            if (_copyMethod.Matched() && !_prob.Matched())
            {
                printf("Copy method require probablity (p) and file (f)\n");
                return 1;
            }

            nm.generate_select_link_or_copy(_copyMethod.Matched(), _prob.Matched() ? _prob.get_actors() : 0.0f);
            nm.export_network(_file.get_actors().c_str());
            return 0;
        }
        else
        {
            nm.generate_random_network(_prob ? _prob.get_actors() : 0, !((bool)_prob));
            printf("Generated network using random method, edge probability=%f.\n", _prob.get_actors());
            nm.export_network(_file.get_actors().c_str());
            printf("Saved network in: %s\n", _file.get_actors().c_str());
            printf("Random network doesn't support full analysis because the network may not be symmetric.\n");
            nm.complete_analysis("Generated using Barabasi-Albert method.", _reportFile.get_actors().c_str(), true, false);
            printf("Completed analysis in %s\n", _reportFile.get_actors().c_str());
            return 0;
        }
    }

    if (_demo)
    {
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
        return 0;
    }

    std::cout << parser;
    return 1;
    */
    return 0;
}
