#pragma once

#include <QDialog>
#include <networkLib/epidemic_models.h>

namespace Ui {
class EpidemicDialog;
}

struct EpidemicParams
{
    azgra::networkLib::epidemic_impl::Model selectedModel;
    uint initialInfected;
    float infectionProb;
    uint recoveryTime;
    uint iterationCount;
    QString resultFileName;
};

class EpidemicDialog : public QDialog
{
Q_OBJECT

public:
    explicit EpidemicDialog(QWidget * parent = nullptr);
    ~EpidemicDialog();

    EpidemicParams get_params() const;

private:
    Ui::EpidemicDialog * ui;
};

