#pragma once
#include <stdio.h>
#include <string>
#include <gtk/gtk.h>
#include <networkLib/network_matrix.h>

struct NetworkInformations
{
    uint vertexCount;
    uint edgeCount;
    float averageDegree;
    float averageClusteringCoefficient;
    float averageDistance;
    uint diameter;
    uint componentCount;
    uint maximumComponentSize;
};

struct ApplicationState
{
    uint age = 5;
    NetworkInformations networkInfoToShow;
    NetworkMatrix network;
};
