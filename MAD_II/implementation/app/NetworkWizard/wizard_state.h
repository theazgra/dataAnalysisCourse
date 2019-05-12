#pragma once
#include <networkLib/network_matrix.h>

#include <QString>

using namespace azgra::networkLib;

enum NetworkSource
{
    NetworkSource_EdgePairs,
    NetworkSource_VectorData,
    NetworkSource_Generated,
    NetworkSource_NoNetwork
};

struct WizardState
{
    bool isNetworkSet = false;
    NetworkSource networkSource = NetworkSource_NoNetwork;
    NetworkMatrix network;
    NetworkReport report;
    QString fileName;
};
