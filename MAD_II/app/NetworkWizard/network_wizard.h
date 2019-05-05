#ifndef NETWORKWIZARD_H
#define NETWORKWIZARD_H

#include <QMainWindow>
#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMovie>
#include "thread_jobs.h"

#include <wizard_state.h>
#include <vertex_info_form.h>


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
    QFutureWatcher<azgra::networkLib::NetworkMatrix> importWatcher;
    QFutureWatcher<azgra::networkLib::NetworkReport> reportWatcher;
    WizardState state;

    void invalidate_loaded_network();
    void show_loader();
    void hide_loader();

    void clear_log();
    void append_to_log(const QString &string);
    void append_to_log(const std::string &string);
    void append_to_log(const char * string);

private slots:
    void import_from_edge_pairs();
    void import_from_vector_data();
    void import_completed();
    void report_created();
    void export_network();
    void clear_network();
    void open_vertex_info();
};

#endif // NETWORKWIZARD_H
