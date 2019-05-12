#ifndef NETWORKWIZARD_H
#define NETWORKWIZARD_H

#include <QMainWindow>
#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMovie>

#include "thread_jobs.h"
#include <import_dialog.h>
#include <wizard_state.h>
#include <vertex_info_form.h>
#include <girvan_newman_dialog.h>
#include <generate_dialog.h>


namespace Ui {
class NetworkWizard;
}

class NetworkWizard : public QMainWindow
{
Q_OBJECT

public:
    explicit NetworkWizard(QWidget * parent = nullptr);
    ~NetworkWizard();

private:
    Ui::NetworkWizard * ui;
    QFutureWatcher<azgra::networkLib::NetworkMatrix> newNetworkWatcher;
    QFutureWatcher<azgra::networkLib::NetworkReport> reportWatcher;
    QFutureWatcher<AlgorithmResult> algorithmWatcher;

    WizardState state;

    void invalidate_loaded_network();
    void show_loader();
    void hide_loader();

    void clear_log();
    void append_to_log_html(const QString &string);
    void append_to_log(const QString &string);
    void log_generate_params(const azgra::networkLib::GeneratorParameters &params);

private slots:
    void import_network();
    void import_from_vector_data();
    void network_load_completed();
    void report_created();
    void export_network();
    void clear_network();
    void open_vertex_info();
    void generate_network();
    void alg_community_girvan_newman();
    void algorithm_completed();
};

#endif // NETWORKWIZARD_H
