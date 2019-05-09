#pragma once
#include <QAbstractTableModel>
#include <networkLib/network_report.h>

class VertexInfoModel : public QAbstractTableModel
{
Q_OBJECT

private:
    azgra::networkLib::NetworkReport report;
    std::vector<uint> communityIds;

public:
    VertexInfoModel(const azgra::networkLib::NetworkReport &report, QObject * parent);

    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void add_community_ids(const std::vector<uint> &communityIds);
};
