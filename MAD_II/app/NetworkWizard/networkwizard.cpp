#include "networkwizard.h"
#include "ui_networkwizard.h"

NetworkWizard::NetworkWizard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NetworkWizard)
{
    ui->setupUi(this);

    connect(ui->miImportFromEdgePairs, &QAction::triggered, this, &NetworkWizard::import_from_edge_pairs);
    connect(&importWatcher, SIGNAL(finished()), this, SLOT(job_completed()));
}

NetworkWizard::~NetworkWizard()
{
    delete ui;
}

void NetworkWizard::import_from_edge_pairs()
{

    //connect(&watcher, SIGNAL(finished()), &myObject, SLOT(handleFinished()));
    ui->textik->setText(QString::number(5000));
    auto jobInfo = QtConcurrent::run(test_thread_concurrency, 5);
    importWatcher.setFuture(jobInfo);
    //jobWatcher.setFuture(jobInfo);

    qDebug() << "MAIN THREAD: worker thread created";
}

void NetworkWizard::job_completed()
{
    int result = importWatcher.result();
    qDebug() << "MAIN THREAD: job completed, result: " << result;
    ui->textik->setText(QString::number(result));
}
