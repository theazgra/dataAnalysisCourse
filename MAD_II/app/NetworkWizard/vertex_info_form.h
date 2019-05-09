#pragma once

#include <QWidget>
#include <QDebug>
#include <vertex_info_model.h>


namespace Ui {
class VertexInfoTable;
}


class VertexInfoTable : public QWidget
{
Q_OBJECT

public:
    explicit VertexInfoTable(const azgra::networkLib::NetworkReport &report, const QString &name,
    QWidget * parent = nullptr);

    explicit VertexInfoTable(const azgra::networkLib::NetworkReport &report, const QString &name,
    const std::vector<uint> &communityIds,
    QWidget * parent = nullptr);

    ~VertexInfoTable();

private:
    Ui::VertexInfoTable * ui;
    VertexInfoModel * model;

    void set_and_show_model();
};
