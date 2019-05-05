#include <vertex_info_model.h>

VertexInfoModel::VertexInfoModel(const azgra::networkLib::NetworkReport& report,
                                 QObject * parent) :
    QAbstractTableModel(parent)
{
    this->report = report;
}

int VertexInfoModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    /*
     *  assert(report.vertexDegrees.size() == report.vertexEccentricities.size()
     *    == report.vertexClosenessCentralities.size() ==
     * report.vertexClusteringCoefficients.size()
     *    == report.vertexCount);
     */
    return static_cast<int>(report.vertexCount);
}

int VertexInfoModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return 5;
}

QVariant VertexInfoModel::data(const QModelIndex& index, int role) const
{
    if ((index.column() > 0) && (role == Qt::TextAlignmentRole))
    {
        return Qt::AlignRight;
    }

    if (!index.isValid() || (role != Qt::DisplayRole))
    {
        return QVariant();
    }
    int v = index.row();
    size_t vId = static_cast<size_t>(v);

    switch (index.column())
    {
        case 0: return static_cast<int>(v);

        case 1: return report.vertexDegrees[vId];

        case 2: return report.vertexEccentricities[vId];

        case 3: return report.vertexClusteringCoefficients[vId];

        case 4: return report.vertexClosenessCentralities[vId];

        default:
            return QVariant();
    }
}

QVariant VertexInfoModel::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const
{
    if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        switch (section)
        {

            case 0: return QString("Vertex Id");

            case 1: return QString("Degree");

            case 2: return QString("Eccentricity");

            case 3: return QString("Clustering coefficient");

            case 4: return QString("Closeness centrality");

            default:
                return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}
