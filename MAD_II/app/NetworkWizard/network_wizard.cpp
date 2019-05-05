#include "network_wizard.h"
#include "ui_network_wizard.h"

NetworkWizard::NetworkWizard(QWidget * parent) :
    QMainWindow(parent),
    ui(new Ui::NetworkWizard)
{
    ui->setupUi(this);

    connect(ui->miImportFromEdgePairs, &QAction::triggered, this, &NetworkWizard::import_from_edge_pairs);
    connect(ui->miImportFromVector, &QAction::triggered, this, &NetworkWizard::import_from_vector_data);
    connect(ui->miClear, &QAction::triggered, this, &NetworkWizard::clear_network);
    connect(ui->miExport, &QAction::triggered, this, &NetworkWizard::export_network);
    connect(ui->miViewVertexInfo, &QAction::triggered, this, &NetworkWizard::open_vertex_info);


    connect(&importWatcher, SIGNAL(finished()), this, SLOT(import_completed()));
    connect(&reportWatcher, SIGNAL(finished()), this, SLOT(report_created()));


    QMovie * loaderGif = new QMovie(":/images/loader.gif");
    ui->loader->setMovie(loaderGif);

    ui->loader->hide();


    #ifndef QT_DEBUG
    qDebug() << "RELEASE VERSION";
    #else
    qDebug() << "DEBUG VERSION";
    #endif
}

NetworkWizard::~NetworkWizard()
{
    delete ui->loader->movie();
    delete ui;
}

void NetworkWizard::import_from_edge_pairs()
{
    QString fName = QFileDialog::getOpenFileName(this, "Import from edge pairs");

    int index = fName.lastIndexOf('/');
    QString fileName = fName.mid(index + 1);

    setWindowTitle(QString("NetworkWizard - %1").arg(fileName));

    if (fName.length() > 0)
    {
        this->state.fileName = fileName;
        show_loader();
        QFuture<azgra::networkLib::NetworkMatrix> importJob = QtConcurrent::run(import_network_from_edges, fName);
        importWatcher.setFuture(importJob);
    }
}

void NetworkWizard::import_from_vector_data()
{
    qDebug() << "called: import_from_vector_data;";
}

void NetworkWizard::import_completed()
{
    azgra::networkLib::NetworkMatrix imported = importWatcher.result();

    this->state.isNetworkSet = true;
    this->state.network = imported;
    this->state.importedFromVectorData = false;


    QString msg = QString("Imported network from %1").arg(state.fileName);
    append_to_log(msg);


    QFuture<azgra::networkLib::NetworkReport> reportJob = QtConcurrent::run(get_network_report_async,
                                                                            this->state.network);
    reportWatcher.setFuture(reportJob);
}

void NetworkWizard::report_created()
{
    azgra::networkLib::NetworkReport report = reportWatcher.result();

    this->state.report = report;
    invalidate_loaded_network();

    VertexInfoTable * vertexInfoForm = new VertexInfoTable(report, QString("Vertex info - %1").arg(state.fileName));
    vertexInfoForm->show();

    hide_loader();
    append_to_log("Created report for imported network");
}

void NetworkWizard::open_vertex_info()
{
    VertexInfoTable * vertexInfoForm = new VertexInfoTable(this->state.report, QString("Vertex info - %1").arg(state.fileName));

    vertexInfoForm->show();
}

void NetworkWizard::invalidate_loaded_network()
{
    bool enable = this->state.isNetworkSet;

    this->ui->miExport->setEnabled(enable);
    this->ui->miViewVertexInfo->setEnabled(enable);

    this->ui->tbVertexCount->setText(QString::number(this->state.report.vertexCount));
    this->ui->tbEdgeCount->setText(QString::number(this->state.report.edgeCount));

    this->ui->tbAvgDegree->setText(QString::number(static_cast<double>(this->state.report.averageDegree)));
    this->ui->tbAvgClustCoeff->setText(QString::number(static_cast<double>(this->state.report.
                                                                           averageClusteringCoefficient)));
    this->ui->tbAvgDist->setText(QString::number(static_cast<double>(this->state.report.averageDistance)));
    this->ui->tbDiameter->setText(QString::number(this->state.report.networkDiameter));

    this->ui->tbComponentCount->setText(QString::number(this->state.report.componentCount));
    this->ui->tbMaxComponentSize->setText(QString::number(this->state.report.maxComponentSize));
}

void NetworkWizard::show_loader()
{
    ui->loader->movie()->start();
    ui->loader->show();
}

void NetworkWizard::hide_loader()
{
    ui->loader->movie()->stop();
    ui->loader->hide();
}

void NetworkWizard::clear_log()
{
    this->ui->logText->clear();
}

void NetworkWizard::append_to_log(const QString&string)
{
    this->ui->logText->appendPlainText(string);
}

void NetworkWizard::append_to_log(const std::string&string)
{
    append_to_log(QString::fromStdString(string));
}

void NetworkWizard::append_to_log(const char * string)
{
    append_to_log(QString(string));
}

void NetworkWizard::export_network()
{
    QString exportFileName = QFileDialog::getSaveFileName(this, "Export current network");

    if (exportFileName.length() > 0)
    {
        state.network.export_network(exportFileName.toStdString().c_str());
        append_to_log(QString("Exported network to %1").arg(exportFileName));
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Export file name wasn't specified!");
        // msgBox.setInformativeText("");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        Q_UNUSED(ret);
    }
}

void NetworkWizard::clear_network()
{
    setWindowTitle("NetworkWizard");
    this->state.isNetworkSet = false;
    this->state.network = azgra::networkLib::NetworkMatrix();
    this->state.report = azgra::networkLib::NetworkReport();

    invalidate_loaded_network();
    append_to_log("Cleared loaded network.");
}
