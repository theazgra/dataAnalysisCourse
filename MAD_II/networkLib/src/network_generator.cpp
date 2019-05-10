#include <networkLib/network_generator.h>
namespace azgra::networkLib
{
NetworkMatrix NetworkGenerator::generate_network(GeneratorParameters params)
{
    assert(params.isSet);
    switch (params.model)
    {
    case NetworkModel_Random:
        return params.symmetric ? SymmetricRandomNetwork(params.finalSize) : RandomNetwork(params.finalSize, params.probability);
    case NetworkModel_BarabasiAlbert:
        return BarabasiAlbert(params.initialSize, params.finalSize, params.newEdgesInStep, params.aging, params.ageScaling);
    case NetworkModel_Bianconi:
        return Bianconi(params.initialSize, params.finalSize, params.newEdgesInStep, params.probability);
    case NetworkModel_HolmeKim:
        return HolmeKim(params.initialSize, params.finalSize, params.newEdgesInStep, params.probability);
    case NetworkModel_Copying:
        return CopyingModel(params.initialSize, params.finalSize, params.probability);
    case NetworkModel_LinkSelection:
        return LinkSelectionModel(params.initialSize, params.finalSize);
    default:
        return NetworkMatrix();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkGenerator::generate_initial_network(const uint initialSize, NetworkMatrix &matrix, std::vector<uint> &vertexList)
{
    assert(matrix.rows() >= initialSize && matrix.cols() >= initialSize);
    assert(vertexList.size() == 0);

    for (uint v = 0; v < initialSize; v++)
    {
        if (v != (initialSize - 1))
        {
            matrix.at(v, v + 1) = 1.0f;
            matrix.at(v + 1, v) = 1.0f;
        }

        vertexList.push_back(v);
        if (v != 0 && v != (initialSize - 1))
        {
            vertexList.push_back(v);
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::RandomNetwork(const uint vertexCount, const float edgeProbability)
{
    return GenerateRandomNetwork(vertexCount, edgeProbability);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::SymmetricRandomNetwork(const uint vertexCount)
{
    float probability = (float)(log(vertexCount) / (float)vertexCount);
    return GenerateRandomNetwork(vertexCount, probability);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::LinkSelectionModel(const uint initialSize, const uint vertexCount)
{
    return GenerateCopyingModelOrLink(initialSize, vertexCount, false, 0);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::CopyingModel(const uint initialSize, const uint vertexCount, const float copyProbability)
{
    return GenerateCopyingModelOrLink(initialSize, vertexCount, true, copyProbability);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::GenerateRandomNetwork(const uint vertexCount, const float edgeProbability)
{
    NetworkMatrix randomNetwork(vertexCount, vertexCount);

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());

    float weights[] = {(1.0f - edgeProbability), edgeProbability};
    std::discrete_distribution<int> discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));

    int edge;
    for (uint row = 0; row < vertexCount; row++)
    {
        for (uint col = row; col < vertexCount; col++)
        {
            if (row != col)
            {
                edge = discreteDistribution(randomGenerator);
                randomNetwork.at(row, col) = edge;
                randomNetwork.at(col, row) = edge;
            }
        }
    }
    return randomNetwork;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::BarabasiAlbert(const uint initialSize, const uint finalSize, const uint newEdgesInStep,
                                               const bool applyAging, const float ageScaling)
{
    NetworkMatrix result(finalSize, finalSize);
    uint currentSize = initialSize;

    std::vector<uint> vertexList;

    generate_initial_network(initialSize, result, vertexList);

    uint newVertexIndex, neighbor;
    float vertexListSize;

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());
    std::discrete_distribution<int> discreteDistribution;

    std::vector<float> weights;
    std::vector<uint> neighbors;

    for (uint step = 0; step < (finalSize - initialSize); step++)
    {
        weights.clear();
        neighbors.clear();

        weights.reserve(currentSize);
        neighbors.reserve(newEdgesInStep);

        newVertexIndex = initialSize + step;
        vertexListSize = (float)vertexList.size();

        for (uint vertex = 0; vertex < currentSize; vertex++)
        {
            float age = (initialSize + step) - vertex;
            float occurence = (float)count(vertexList, vertex);
            float weight = (occurence / vertexListSize);
            float agedWeight = weight * pow(age, (-1.0f * ageScaling));

            weights.push_back(applyAging ? agedWeight : weight);
        }

        discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));

        for (uint neighbourStep = 0; neighbourStep < newEdgesInStep; neighbourStep++)
        {

            neighbor = discreteDistribution(randomGenerator);
            while (find(neighbors, neighbor))
            {
                neighbor = discreteDistribution(randomGenerator);
            }
            neighbors.push_back(neighbor);
        }

        assert(neighbors.size() == newEdgesInStep);

        vertexList.push_back(newVertexIndex);
        vertexList.push_back(newVertexIndex);

        for (const uint &newNeighbour : neighbors)
        {
            vertexList.push_back(newNeighbour);
            result.at(newVertexIndex, newNeighbour) = 1.0;
            result.at(newNeighbour, newVertexIndex) = 1.0;
        }

        currentSize++;
    }

    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::HolmeKim(const uint initialSize, const uint finalSize, const uint newEdgesInStep, const float probability)
{
    NetworkMatrix result(finalSize, finalSize);
    uint currentSize = initialSize;

    std::vector<uint> vertexList;
    generate_initial_network(initialSize, result, vertexList);

    uint newVertexIndex, neighbor;
    float vertexListSize;

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());
    std::discrete_distribution<int> discreteDistribution;

    std::discrete_distribution<int> chooseNeighborOfNeighbor({1 - probability, probability});

    std::vector<float> weights;
    std::vector<uint> neighbors;
    for (uint step = 0; step < (finalSize - initialSize); step++)
    {
        weights.clear();
        neighbors.clear();

        weights.reserve(currentSize);
        neighbors.reserve(newEdgesInStep);

        newVertexIndex = initialSize + step;
        vertexListSize = (float)vertexList.size();

        for (uint vertex = 0; vertex < currentSize; vertex++)
        {
            weights.push_back(((float)count(vertexList, vertex) / vertexListSize));
        }

        discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));
        bool triadFormation = false;
        uint lastConnectedVertex = 0;
        for (uint neighbourStep = 0; neighbourStep < newEdgesInStep; neighbourStep++)
        {
            if (neighbourStep == 0) // In first step always use PA.
                triadFormation = false;
            else
            {
                int triadFormRandom = chooseNeighborOfNeighbor(randomGenerator);
                if (triadFormRandom) // Choose some random neighbor of last connected vertex
                    triadFormation = true;
                else // Do another pa step.
                    triadFormation = false;
            }

            auto lastConnectedNeighbors = result.get_neighbors(lastConnectedVertex);
            if (lastConnectedNeighbors.size() == 0)
            {
                // If there are no vertices to connect to do PA step.
                triadFormation = false;
            }

            if (triadFormation)
            {

                std::uniform_int_distribution<int> randNeigh(0, lastConnectedNeighbors.size() - 1);
                do
                {
                    neighbor = lastConnectedNeighbors[randNeigh(randomGenerator)];
                } while (find(neighbors, neighbor));

                neighbors.push_back(neighbor);
                lastConnectedVertex = neighbor;
            }
            else
            {
                do
                {
                    neighbor = discreteDistribution(randomGenerator);
                } while (find(neighbors, neighbor));
                neighbors.push_back(neighbor);
                lastConnectedVertex = neighbor;
            }
        }

        assert(neighbors.size() == newEdgesInStep);

        vertexList.push_back(newVertexIndex);
        vertexList.push_back(newVertexIndex);

        for (const uint &newNeighbour : neighbors)
        {
            vertexList.push_back(newNeighbour);
            result.at(newVertexIndex, newNeighbour) = 1.0;
            result.at(newNeighbour, newVertexIndex) = 1.0;
        }

        currentSize++;
    }

    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::Bianconi(const uint initialSize, const uint finalSize, const uint newEdgesInStep, const float probability)
{
    NetworkMatrix result(finalSize, finalSize);
    {
        std::vector<uint> tmp;
        generate_initial_network(initialSize, result, tmp);
    }

    uint newVertexIndex, neighbor;

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());

    std::discrete_distribution<int> chooseNeighborOfNeighbor({1 - probability, probability});
    std::vector<uint> neighbors;
    std::uniform_int_distribution<> randomVertexGenerator;
    for (uint step = 0; step < (finalSize - initialSize); step++)
    {
        neighbors.clear();
        neighbors.reserve(newEdgesInStep);

        newVertexIndex = initialSize + step;
        randomVertexGenerator = std::uniform_int_distribution<>(0, newVertexIndex - 1);
        uint newRandomNeigh = randomVertexGenerator(randomGenerator);

        neighbors.push_back(newRandomNeigh);
        uint lastConnectedVertex = newRandomNeigh;

        for (uint neighbourStep = 0; neighbourStep < newEdgesInStep - 1; neighbourStep++)
        {
            bool randomNetworkNode = true;

            std::vector<uint> lastConnectedNeighbors;
            if (chooseNeighborOfNeighbor(randomGenerator))
            {
                lastConnectedNeighbors = result.get_neighbors(lastConnectedVertex);
                randomNetworkNode = !(lastConnectedNeighbors.size() > 0);
            }

            if (randomNetworkNode)
            {
                do
                {
                    neighbor = randomVertexGenerator(randomGenerator);
                } while (find(neighbors, neighbor));

                neighbors.push_back(neighbor);
                lastConnectedVertex = neighbor;
            }
            else // Choose some random neighbor of neighbor
            {

                std::uniform_int_distribution<> randNeigh(0, lastConnectedNeighbors.size() - 1);
                do
                {
                    neighbor = lastConnectedNeighbors[randNeigh(randomGenerator)];
                } while (find(neighbors, neighbor));

                neighbors.push_back(neighbor);
                lastConnectedVertex = neighbor;
            }
        }

        assert(neighbors.size() == newEdgesInStep);

        for (const uint &newNeighbour : neighbors)
        {
            result.at(newVertexIndex, newNeighbour) = 1.0;
            result.at(newNeighbour, newVertexIndex) = 1.0;
        }
    }

    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix NetworkGenerator::GenerateCopyingModelOrLink(const uint initialSize, const uint finalSize,
                                                           const bool copy, const float copyProbability)
{

    NetworkMatrix result(finalSize, finalSize);
    {
        std::vector<uint> tmp;
        generate_initial_network(initialSize, result, tmp);
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> neighRandom;
    std::uniform_int_distribution<int> linkEndRandom(0, 1);
    std::discrete_distribution<int> copyRandom({1 - copyProbability, copyProbability});

    std::vector<std::pair<uint, uint>> edges = result.get_edges();

    std::pair<uint, uint> edge;
    uint newVertexIndex;
    for (uint step = 0; step < (finalSize - initialSize); step++)
    {
        newVertexIndex = initialSize + step;
        std::uniform_int_distribution<int> edgeRandom(0, (int)(edges.size() - 1));
        edge = edges[edgeRandom(rd)];

        uint partner;
        if (copy)
        {
            // Copy wth probability select edge.first with (1-probability) select edge.first random neigbor.
            if (copyRandom(rd))
            {
                // Select edge.first
                partner = edge.first;
            }
            else
            {
                // Select random neighbor of edge.first
                auto neighbors = result.get_neighbors(edge.first);
                neighRandom = std::uniform_int_distribution<int>(0, (int)(neighbors.size() - 1));
                partner = neighbors[neighRandom(rd)];
            }
        }
        else
        {
            // Link selection, select one of vertices on the edge.
            partner = linkEndRandom(rd) ? edge.first : edge.second;
        }

        result.at(newVertexIndex, partner) = 1.0f;
        result.at(partner, newVertexIndex) = 1.0f;
        edges.push_back(std::make_pair(newVertexIndex, partner));
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}; // namespace azgra::networkLib