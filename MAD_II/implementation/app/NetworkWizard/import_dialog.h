#pragma once

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class ImportDialog;
}

struct ImportSettings
{
    QString path;
    QString delimiter;
    bool preprocess;
};

class ImportDialog : public QDialog
{
Q_OBJECT

public:
    explicit ImportDialog(QWidget * parent = nullptr);
    ~ImportDialog();

    ImportSettings get_settings() const;

private:
    Ui::ImportDialog * ui;

private slots:
    void select_path_dialog();
};

