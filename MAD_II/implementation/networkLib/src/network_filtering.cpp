#include <networkLib/network_filtering.h>

namespace azgra::networkLib
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix filter_e_radius(const NetworkMatrix &src, const float radius)
{
    NetworkMatrix result = NetworkMatrix(src);

    for (size_t i = 0; i < src.rows() * src.cols(); i++)
    {
        if (src.at(i) >= radius)
        {
            result.at(i) = 1.0f;
        }
        else
        {
            result.at(i) = 0.0f;
        }
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix filter_kNN(const NetworkMatrix &src, const uint k)
{
    NetworkMatrix result(src);
    for (uint row = 0; row < src.rows(); row++)
    {
        //TODO: Should we use src or result in find_k_neighbors call?
        auto kNeighbors = result.find_k_neighbors(row, k);

        for (uint col = 0; col < src.cols(); col++)
        {
            result.at(row, col) = 0.0f;
        }
        for (const uint &neigh : kNeighbors)
            result.at(row, neigh) = 1.0f;
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix filter_combinataion_e_knn(const NetworkMatrix &src, const float radius, const uint k)
{
    NetworkMatrix result(src);
    uint inRadiusCount = 0;
    for (uint row = 0; row < src.rows(); row++)
    {
        //TODO: Should we use src or result in count call?
        inRadiusCount = src.count_in_e_radius(row, radius);
        if (inRadiusCount > k)
        {
            //Take in radius.
            for (uint col = 0; col < src.cols(); col++)
            {
                if (result.at(row, col) >= radius)
                    result.at(row, col) = 1.0f;
                else
                    result.at(row, col) = 0.0f;
            }
        }
        else
        {
            // Take K neighbors.
            result.filter_knn_row(row, k);
        }
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix filter_random_node_sampling(const NetworkMatrix &src, const float targetPercentSize)
{
    //printf("Random based sampling\n");
    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());

    float weights[] = {(1.0f - targetPercentSize), targetPercentSize};
    std::discrete_distribution<int> discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));

    bool isInSample;
    std::vector<uint> sample;
    sample.reserve(src.rows());

    uint threshold = (uint)((float)src.rows() * targetPercentSize);

    for (size_t i = 0; i < src.rows(); i++)
    {
        isInSample = discreteDistribution(randomGenerator) == 1;
        if (isInSample)
        {
            sample.push_back(i);
            for (const uint n : src.get_neighbors(i))
            {
                if (!find(sample, n))
                    sample.push_back(n);
            }
        }
        if (sample.size() > threshold)
            break;
    }
    size_t sampleSize = sample.size();
    NetworkMatrix sampleNetwork = NetworkMatrix(sampleSize, sampleSize);

    uint u, v;
    for (uint sampleRow = 0; sampleRow < sampleSize; sampleRow++)
    {
        u = sample[sampleRow];
        for (uint sampleCol = sampleRow + 1; sampleCol < sampleSize; sampleCol++)
        {
            v = sample[sampleCol];
            if (src.is_edge_at(u, v))
            {
                sampleNetwork.at(sampleRow, sampleCol) = 1.0f;
            }
        }
    }

    // printf("Random Node sampling results: VC: %5u EC: %5u\n", sampleNetwork.vertex_count(), sampleNetwork.edge_count());
    return sampleNetwork;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix filter_random_edge_sampling(const NetworkMatrix &src, const float targetPercentSize)
{

    printf("Degree based sampling\n");
    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());
    std::discrete_distribution<int> discreteDistribution;

    std::vector<uint> sample;
    sample.reserve(src.rows());

    auto degrees = src.get_degree_of_vertices();

    uint threshold = (uint)((float)src.rows() * targetPercentSize);
    float weights[2];
    float prob;
    for (size_t row = 0; row < src.rows(); row++)
    {
        prob = targetPercentSize / (float)degrees[row];
        weights[0] = 1.0f - prob;
        weights[1] = prob;
        discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));
        if (discreteDistribution(randomGenerator) == 1)
        {
            //sample.push_back(row);
            sample.push_back(row);
            for (const uint n : src.get_neighbors(row))
            {
                if (!find(sample, n))
                    sample.push_back(n);
            }

            if (sample.size() > threshold)
                break;
        }
    }
    size_t sampleSize = sample.size();
    NetworkMatrix sampleNetwork(sampleSize, sampleSize);

    uint u, v;
    for (uint sampleRow = 0; sampleRow < sampleSize; sampleRow++)
    {
        u = sample[sampleRow];
        for (uint sampleCol = sampleRow + 1; sampleCol < sampleSize; sampleCol++)
        {
            v = sample[sampleCol];
            if (src.is_edge_at(u, v))
            {
                sampleNetwork.at(sampleRow, sampleCol) = 1.0f;
            }
        }
    }

    //printf("Random Edge Sampling results: VC: %5u EC: %5u\n", sampleNetwork.vertex_count(), sampleNetwork.edge_count());
    return sampleNetwork;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix filter_k_core(const NetworkMatrix &src, const uint k)
{
    NetworkMatrix result(src);
    assert(src.rows() == result.rows());

    std::vector<uint> kCoreVertices;
    kCoreVertices.resize(src.rows());

    for (size_t i = 0; i < result.rows(); i++)
        kCoreVertices[i] = i;

    bool reduce = true;
    while (reduce)
    {
        reduce = false;
        std::vector<uint> tmpKCoreVertices;
        std::vector<uint> degrees = result.get_degree_of_vertices();

        for (const uint &v : kCoreVertices)
        {
            if (degrees[v] >= k)
            {
                tmpKCoreVertices.push_back(v);
            }
            else
            {
                result.delete_edges_for(v);
                reduce = true;
            }
        }

        kCoreVertices.swap(tmpKCoreVertices);
    }
    //printf("Final vertex count %lu\n", kCoreVertices.size());
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void kernighan_lin(const NetworkMatrix &src)
{
    uint rowCount = src.rows();

    const size_t groupCount = 2;
    int groupSize = (rowCount + groupCount - 1) / 2;
    std::vector<uint> alreadySwapped;
    alreadySwapped.reserve(rowCount);

    std::vector<uint> groupA;
    std::vector<uint> groupB;
    groupA.reserve(groupSize);
    groupB.reserve(groupSize);

    std::vector<uint> indices;
    indices.resize(rowCount);
    for (size_t i = 0; i < rowCount; i++)
        indices[i] = i;

    std::random_shuffle(indices.begin(), indices.end());

    for (uint i = 0; i < rowCount; i++)
    {
        if (i % 2 == 0)
            groupA.push_back(indices[i]);
        else
            groupB.push_back(indices[i]);
    }

    uint lastCut = src.get_edge_count_between_groups(groupA, groupB);
    uint cut = lastCut;
    int iter = 0;

    do
    {
        // one step.
        std::vector<SwapInfo> stepSwaps;
        for (size_t a = 0; a < groupA.size(); a++)
        {
            for (size_t b = 0; b < groupB.size(); b++)
            {
                uint vA = groupA[a];
                uint vB = groupB[b];

                if (find(alreadySwapped, vA) || find(alreadySwapped, vB))
                    continue;

                groupA[a] = vB;
                groupB[b] = vA;

                uint cut = src.get_edge_count_between_groups(groupA, groupB);
                stepSwaps.push_back(SwapInfo(a, b, cut));

                groupA[a] = vA;
                groupB[b] = vB;
            }
        }

        std::sort(stepSwaps.begin(), stepSwaps.end());
        SwapInfo bestSwap = stepSwaps[0];
        // Swap them.
        uint vA = groupA[bestSwap.aIndex];
        uint vB = groupB[bestSwap.bIndex];
        groupA[bestSwap.aIndex] = vB;
        groupB[bestSwap.bIndex] = vA;
        alreadySwapped.push_back(vA);
        alreadySwapped.push_back(vB);

        lastCut = cut;
        cut = src.get_edge_count_between_groups(groupA, groupB);

        if (cut > lastCut)
        {
            groupA[bestSwap.aIndex] = vA;
            groupB[bestSwap.bIndex] = vB;
            cut = src.get_edge_count_between_groups(groupA, groupB);
            printf("Correcting result. Swapping pair back. Final cut: %4u\n", cut);
            break;
        }

        printf("Finished iterataion %i; Went from %4u to %4u.\n", ++iter, lastCut, cut);
    } while (cut < lastCut);

    printf("First group: ");
    print_vector(groupA);
    printf("Second group: ");
    print_vector(groupB);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix simulate_failure(const NetworkMatrix &src, const uint stepCount)
{
    NetworkMatrix result(src);
    for (size_t attackStep = 0; attackStep < stepCount; attackStep++)
    {
        result.failure_step();
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NetworkMatrix simulate_attack(const NetworkMatrix &src, const uint stepCount)
{
    NetworkMatrix result(src);
    for (size_t attackStep = 0; attackStep < stepCount; attackStep++)
    {
        result.attack_step();
    }
    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}; // namespace azgra::networkLib
