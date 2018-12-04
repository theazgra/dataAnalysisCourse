#include "external/args.hxx"
#include "network_matrix.h"

/*
    NetworkMatrix is always indexed from zero!!!
    - If imported network vertex begin at one instead of zero use offset of -1 in NetworkMatrix::load_from_edges() function.

    Network can be either imported or generated. Import usage is now commented out.

    Two network types can be generated. Random ( generate_random_network() ) or Barabasi-Albert model ( generate_scale_free_network ).


*/
int main(int argc, char **argv)
{

    args::ArgumentParser parser("Network analysis. - MOR0146");
    args::Group methodGroup(parser, "Methods", args::Group::Validators::AtMostOne);
    args::HelpFlag _help(methodGroup, "help", "Print help", {'h', "help"});

    args::Flag _import(methodGroup, "import", "Import network from csv file. <file>", {'i', "import"});
    args::Flag _generate(methodGroup, "generate", "Generate and save network. <method> <vertex-count> <network-file> <report-file> [probability]",
                         {'g', "generate"});

    args::Flag _demo(methodGroup, "demo", "Generate and analyse Network model generated by Barabasi-Albert model.", {'d', "demo"});

    args::ValueFlag<std::string> _file(parser, "file", "CSV file.", {'f', "file"});
    args::ValueFlag<std::string> _reportFile(parser, "file", "CSV file.", {'f', "file"});
    args::ValueFlag<uint> _vertexCount(parser, "vertex-count", "Vertex count..", {'v', "vertex-count"});

    args::Flag _display(parser, "BA", "Use Barabasi-Albert model for generating..", {"ba"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    if (_generate)
    {
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
    return 0;

    /*
    char *csvFile = "/home/mor0146/github/dataAnalysisCourse/data/KarateClub.csv";
    auto karateEdges = load_edge_pairs(csvFile, ";");
    uint vc = get_vertex_count_from_edge_pairs(karateEdges);
    NetworkMatrix network = NetworkMatrix(vc, vc);
    network.load_from_edges(karateEdges);
    network.complete_analysis("Zachary-karate-club", "../out/zacharyReport.txt");
    */
}
