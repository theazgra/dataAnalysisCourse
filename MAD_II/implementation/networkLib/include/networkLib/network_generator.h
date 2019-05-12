#pragma once
#include "network_matrix.h"
#include "network_generator_params.h"
#include <cassert>

namespace azgra::networkLib
{

class NetworkGenerator
{
private:
    static void generate_initial_network(const uint initialSize, NetworkMatrix &matrix, std::vector<uint> &vertexList);

    static NetworkMatrix GenerateCopyingModelOrLink(const uint initialSize, const uint vertexCount, const bool copy, const float copyProbability);

    static NetworkMatrix RandomNetwork(const uint vertexCount, const float edgeProbability);

    static NetworkMatrix SymmetricRandomNetwork(const uint vertexCount);

    static NetworkMatrix GenerateRandomNetwork(const uint vertexCount, const float edgeProbability);

    static NetworkMatrix BarabasiAlbert(const uint initialSize, const uint finalVertexCount, const uint newEdgesInStep,
                                        const bool applyAging = false, const float ageScaling = 0.0f);

    static NetworkMatrix HolmeKim(const uint initialSize, const uint vertexCount, const uint newEdgesInStep, const float probability);

    static NetworkMatrix Bianconi(const uint initialSize, const uint vertexCount, const uint newEdgesInStep, const float probability);

    static NetworkMatrix LinkSelectionModel(const uint initialSize, const uint vertexCount);

    static NetworkMatrix CopyingModel(const uint initialSize, const uint vertexCount, const float copyProbability);

public:
    /**
     * @brief Generate random network.
     * 
     * @param params Model parameters.
     * @return NetworkMatrix Generated network.
     */
    static NetworkMatrix generate_network(GeneratorParameters params);
};

}; // namespace azgra::networkLib