// #include <vertex_info_form.h>
// #include "vertexinfotable.h"
#include <vertex_info_form.h>
#include "ui_vertex_info_form.h"

VertexInfoTable::VertexInfoTable(const azgra::networkLib::NetworkReport&report, const QString&name, QWidget * parent) :
    QWidget(parent),
    ui(new Ui::VertexInfoTable)
{
    ui->setupUi(this);
    setWindowTitle(name);
    setAttribute(Qt::WA_DeleteOnClose);

    model = new VertexInfoModel(report, this);
    set_and_show_model();
}

VertexInfoTable::VertexInfoTable(const azgra::networkLib::NetworkReport &report,
    const QString &name,
    const std::vector<uint> &communityIds,
    QWidget * parent) : QWidget(parent), ui(new Ui::VertexInfoTable)
{
    ui->setupUi(this);
    setWindowTitle(name);
    setAttribute(Qt::WA_DeleteOnClose);

    model = new VertexInfoModel(report, this);
    model->add_community_ids(communityIds);

    set_and_show_model();
}

VertexInfoTable::~VertexInfoTable()
{
    delete model;
    delete ui;
    qDebug() << "VertexInfoTable: Destructor called";
}

void VertexInfoTable::set_and_show_model()
{
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table->setModel(model);
    ui->table->horizontalHeader()->setVisible(true);
    ui->table->show();
}
