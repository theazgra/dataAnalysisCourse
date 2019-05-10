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
    connect(ui->miGenerate, &QAction::triggered, this, &NetworkWizard::generate_network);
    connect(ui->miGirvanNewman, &QAction::triggered, this, &NetworkWizard::alg_community_girvan_newman);

    connect(&newNetworkWatcher, SIGNAL(finished()), this, SLOT(network_load_completed()));
    connect(&reportWatcher, SIGNAL(finished()), this, SLOT(report_created()));
    connect(&algorithmWatcher, SIGNAL(finished()), this, SLOT(algorithm_completed()));


    QMovie * loaderGif = new QMovie(":/images/loader.gif");
    ui->loader->setMovie(loaderGif);

    ui->loader->hide();
    invalidate_loaded_network();


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
        this->state.networkSource = NetworkSource_EdgePairs;

        show_loader();
        QFuture<azgra::networkLib::NetworkMatrix> importJob = QtConcurrent::run(import_network_from_edges, fName);
        newNetworkWatcher.setFuture(importJob);
    }
}

void NetworkWizard::generate_network()
{
    GenerateDialog * genDialog = new GenerateDialog();

    int result = genDialog->exec();

    if (result == QDialog::Accepted)
    {
        auto params = genDialog->get_generator_params();

        azgra::SimpleString modelName = azgra::networkLib::get_network_model_name(params.model);

        setWindowTitle(QString("NetworkWizard - %1").arg(modelName.get_c_string()));


        log_generate_params(params);

        this->state.fileName.clear();
        this->state.networkSource = NetworkSource_Generated;

        show_loader();
        QFuture<azgra::networkLib::NetworkMatrix> generateJob = QtConcurrent::run(generate_network_async, params);
        newNetworkWatcher.setFuture(generateJob);
    }

    delete genDialog;
}

void NetworkWizard::log_generate_params(const azgra::networkLib::GeneratorParameters &params)
{
    // append_to_log(QString("Generating network using %1 ...").arg(modelName.get_c_string()));
    switch (params.model)
    {
        case azgra::networkLib::NetworkModel_Random:
        {
            append_to_log_html(QString("<p><b>Generate random network</b></p>"
                                       "<p>  Final size: %1</p>"
                                       "<p>  Edge Probablity: %2</p>"
                                       "<p>  Symmetric: %3</p>")
                               .arg(params.finalSize)
                               .arg(static_cast<double>(params.probability))
                               .arg(params.symmetric));
        }
        break;
        case azgra::networkLib::NetworkModel_BarabasiAlbert:
        {
            append_to_log_html(QString("<p><b>Generate Barabasi-Albert model</b></p>"
                                       "<p>  Initial size: %1</p>"
                                       "<p>  Final size: %2</p>"
                                       "<p>  New edges in step: %3</p>"
                                       "<p>  Apply aging: %4</p>"
                                       "<p>  Age scaling: %5</p>")
                               .arg(params.initialSize)
                               .arg(params.finalSize)
                               .arg(params.newEdgesInStep)
                               .arg(params.aging)
                               .arg(static_cast<double>(params.ageScaling)));
        }
        break;
        case azgra::networkLib::NetworkModel_Bianconi:
        {
            append_to_log_html(QString("<p><b>Generate Bianconi model</b></p>"
                                       "<p>  Initial size: %1</p>"
                                       "<p>  Final size: %2</p>"
                                       "<p>  New edges in step: %3</p>"
                                       "<p>  Choose random neigbor of neighbor probablity: %4</p>")
                               .arg(params.initialSize)
                               .arg(params.finalSize)
                               .arg(params.newEdgesInStep)
                               .arg(static_cast<double>(params.probability)));
        }
        break;
        case azgra::networkLib::NetworkModel_HolmeKim:
        {
            append_to_log_html(QString("<p><b>Generate Holme-Kim model</b></p>"
                                       "<p>  Initial size: %1</p>"
                                       "<p>  Final size: %2</p>"
                                       "<p>  New edges in step: %3</p>"
                                       "<p>  Choose random neigbor of neighbor probablity: %4</p>")
                               .arg(params.initialSize)
                               .arg(params.finalSize)
                               .arg(params.newEdgesInStep)
                               .arg(static_cast<double>(params.probability)));
        }
        break;
        case azgra::networkLib::NetworkModel_Copying:
        {
            append_to_log_html(QString("<p><b>Generate Copying model</b></p>"
                                       "<p>  Initial size: %1</p>"
                                       "<p>  Final size: %2</p>"
                                       "<p>  Copy probability: %3</p>")
                               .arg(params.initialSize)
                               .arg(params.finalSize)
                               .arg(static_cast<double>(params.probability)));
        }
        break;
        case azgra::networkLib::NetworkModel_LinkSelection:
        {
            append_to_log_html(QString("<p><b>Generate Link selection model</b></p>"
                                       "<p>  Initial size: %1</p>"
                                       "<p>  Final size: %2</p>")
                               .arg(params.initialSize)
                               .arg(params.finalSize));
        }
        break;
        default:
            break;
    }
}


void NetworkWizard::alg_community_girvan_newman()
{
    GirvanNewmanDialog * algDialog = new GirvanNewmanDialog();
    int result = algDialog->exec();

    if (result == QDialog::Accepted)
    {
        GirvanNewmanSettings settings = algDialog->get_settings();
        append_to_log_html(QString("<p><b>Selected Girvan-Newman clustering algorithm to find communities</b></p>"
                                   "<p>  Settings:</p>"
                                   "<p>    Max iteration count: %1</p>"
                                   "<p>    Target modularity: %2</p>"
                                   "<p>    Save result to: %3</p>")
                           .arg(settings.maxIterationCount)
                           .arg(static_cast<double>(settings.targetModularity))
                           .arg(settings.resultFileName));


        QFuture<AlgorithmResult> algorithmJob = QtConcurrent::run(girvan_newman_async,
                                                                  this->state.network,
                                                                  settings.maxIterationCount,
                                                                  settings.targetModularity,
                                                                  settings.resultFileName);

        algorithmWatcher.setFuture(algorithmJob);
        show_loader();

    }
    delete algDialog;
}

void NetworkWizard::import_from_vector_data()
{
}

void NetworkWizard::network_load_completed()
{
    azgra::networkLib::NetworkMatrix imported = newNetworkWatcher.result();

    this->state.isNetworkSet = true;
    this->state.network = imported;

    switch (this->state.networkSource)
    {
        case NetworkSource_EdgePairs:
        {
            QString msg = QString("Imported network edge pairs from %1").arg(state.fileName);
            append_to_log(msg);
        }
        break;
        case NetworkSource_VectorData:
        {
            QString msg = QString("Imported network vector data from %1").arg(state.fileName);
            append_to_log(msg);
        }
        break;
        case NetworkSource_Generated:
        {
            append_to_log("Generated network...");
        }
        break;
        case NetworkSource_NoNetwork:
            break;
    }

    append_to_log("Starting network analysis...");
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
    append_to_log("Network report created.");
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
    this->ui->menuAlgorithms->setEnabled(enable);
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

void NetworkWizard::append_to_log_html(const QString &string)
{
    this->ui->logText->appendHtml(string);
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

void NetworkWizard::algorithm_completed()
{
    hide_loader();
    AlgorithmResult result = algorithmWatcher.result();

    append_to_log(result.log);

    switch (result.alg)
    {
        case Alg_GirvanNewman:
        {
            append_to_log(QString("Found communities after %1 ms").arg(result.elapsedMilliseconds));
            auto lastIterResult = result.communityEvolveResult[result.communityEvolveResult.size() - 1];

            std::vector<uint> communityIds = azgra::networkLib::CommunityFinder::get_vertex_community_ids(this->state.network.rows(), lastIterResult);
            VertexInfoTable * vertexInfoForm = new VertexInfoTable(this->state.report,
                                                                   QString("Vertex info - Communities"),
                                                                   communityIds);

            // TODO: Display community info window with table like this
            // Community Id | Vertex count | Modularity | Inner edge count
            vertexInfoForm->show();

        }
        break;
    }
}
