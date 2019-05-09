#pragma once

#include <QDialog>

namespace Ui {
class GirvanNewmanDialog;
}

struct GirvanNewmanSettings
{
    int maxIterationCount = 1;
    float targetModularity = 0.0f;
};

class GirvanNewmanDialog : public QDialog
{
Q_OBJECT

public:
    explicit GirvanNewmanDialog(QWidget * parent = nullptr);
    ~GirvanNewmanDialog();

    GirvanNewmanSettings get_settings() const;

private:
    Ui::GirvanNewmanDialog * ui;
};

