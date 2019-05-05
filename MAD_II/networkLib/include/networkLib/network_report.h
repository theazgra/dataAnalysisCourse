#pragma once

#include <networkLib/types.h>
#include <vector>

namespace azgra::networkLib
{
struct NetworkReport
{
    uint vertexCount;
    uint edgeCount;
    float averageDegree;
    float averageClusteringCoefficient;
    float averageDistance;
    uint networkDiameter;
    uint componentCount;
    uint maxComponentSize;

    std::vector<uint> vertexDegrees;
    std::vector<float> vertexEccentricities;
    std::vector<float> vertexClusteringCoefficients;
    std::vector<float> vertexClosenessCentralities;

    NetworkReport()
    {
        vertexCount = 0;
        edgeCount = 0;
        averageDegree = 0;
        averageClusteringCoefficient = 0;
        averageDistance = 0;
        networkDiameter = 0;
        componentCount = 0;
        maxComponentSize = 0;
    }
};

struct ReportRequest
{
    bool includeVertexStats;
};
}; // namespace azgra::networkLib