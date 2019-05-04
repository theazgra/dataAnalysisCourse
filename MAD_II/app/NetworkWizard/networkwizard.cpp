#include "networkwizard.h"
#include "ui_networkwizard.h"

NetworkWizard::NetworkWizard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NetworkWizard)
{
    ui->setupUi(this);

    connect(ui->miImportFromEdgePairs, &QAction::triggered, this, &NetworkWizard::import_from_edge_pairs);
    connect(&importWatcher, SIGNAL(finished()), this, SLOT(import_completed()));
}

NetworkWizard::~NetworkWizard()
{
    delete ui;
}

void NetworkWizard::import_from_edge_pairs()
{
    QString fName = QFileDialog::getOpenFileName(this, "Import from edge pairs");
    if (fName.length() > 0)
    {
        QFuture<azgra::networkLib::NetworkMatrix> importJob = QtConcurrent::run(import_network_from_edges, fName);
        importWatcher.setFuture(importJob);
    }
}

void NetworkWizard::import_completed()
{
    azgra::networkLib::NetworkMatrix imported = importWatcher.result();
    this->mainNetwork = imported;
    qDebug() << "Imported network with " << this->mainNetwork.vertex_count() << " vertices.";

}
