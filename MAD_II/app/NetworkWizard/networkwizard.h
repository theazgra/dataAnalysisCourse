#ifndef NETWORKWIZARD_H
#define NETWORKWIZARD_H

#include <QMainWindow>
#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include "thread_test.h"

#include <networkLib/network_matrix.h>


namespace Ui {
class NetworkWizard;
}

class NetworkWizard : public QMainWindow
{
    Q_OBJECT

public:
    explicit NetworkWizard(QWidget *parent = nullptr);
    ~NetworkWizard();

private:
    Ui::NetworkWizard *ui;
    azgra::networkLib::NetworkMatrix mainNetwork;
    QFutureWatcher<azgra::networkLib::NetworkMatrix> importWatcher;

private slots:
    void import_from_edge_pairs();
    void import_completed();
};

#endif // NETWORKWIZARD_H
