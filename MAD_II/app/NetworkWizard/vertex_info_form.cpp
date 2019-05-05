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

    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    model = new VertexInfoModel(report, this);
    ui->table->setModel(model);
    ui->table->horizontalHeader()->setVisible(true);
    ui->table->show();
}

VertexInfoTable::~VertexInfoTable()
{
    delete model;
    delete ui;
    qDebug() << "VertexInfoTable: Destructor called";
}
