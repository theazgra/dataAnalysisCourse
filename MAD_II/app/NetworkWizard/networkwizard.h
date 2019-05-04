#ifndef NETWORKWIZARD_H
#define NETWORKWIZARD_H

#include <QMainWindow>
#include <QDebug>
#include <QtConcurrent>
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
    QFutureWatcher<int> importWatcher;
    NetworkMatrix nm;

private slots:
    void import_from_edge_pairs();
    void job_completed();
};

#endif // NETWORKWIZARD_H
