#pragma once
#include <networkLib/network_matrix.h>

#include <QString>

using namespace azgra::networkLib;

struct WizardState
{
    bool isNetworkSet = false;
    bool importedFromVectorData = false;
    NetworkMatrix network;
    NetworkReport report;
    QString fileName;
};
