#pragma once
#include <QDebug>
#include <networkLib/network_matrix.h>

using namespace azgra::networkLib;

inline azgra::networkLib::NetworkMatrix import_network_from_edges(const QString &fileName)
{
    NetworkMatrix result(fileName.toStdString().c_str());
    return result;
}
