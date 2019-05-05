#pragma once
#include <QDebug>
#include <networkLib/network_matrix.h>

using namespace azgra::networkLib;

inline NetworkMatrix import_network_from_edges(const QString &fileName)
{
    NetworkMatrix result(fileName.toStdString().c_str());
    return result;
}

inline NetworkReport get_network_report_async(const NetworkMatrix &network)
{
    ReportRequest request = {};
    request.includeVertexStats = true;
    NetworkReport report  = network.get_network_report(request);
    return report;
}
