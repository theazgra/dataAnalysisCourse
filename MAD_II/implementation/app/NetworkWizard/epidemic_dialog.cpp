#include "epidemic_dialog.h"
#include "ui_epidemic_dialog.h"

EpidemicDialog::EpidemicDialog(QWidget * parent) :
    QDialog(parent),
    ui(new Ui::EpidemicDialog)
{
    ui->setupUi(this);
}

EpidemicDialog::~EpidemicDialog()
{
    delete ui;
}

EpidemicParams EpidemicDialog::get_params() const
{
    EpidemicParams params = {};

    azgra::networkLib::epidemic_impl::Model selectedModel = azgra::networkLib::epidemic_impl::Model_SI;

    if (ui->rbSIS->isChecked())
    {
        selectedModel = azgra::networkLib::epidemic_impl::Model_SIS;
    }
    else if (ui->rbSIR->isChecked())
    {
        selectedModel = azgra::networkLib::epidemic_impl::Model_SIR;
    }

    params.selectedModel = selectedModel;
    params.initialInfected = static_cast<uint>(ui->tbInitialInfected->value());
    params.infectionProb = static_cast<float>(ui->tbInfectionProb->value());
    params.recoveryTime = static_cast<uint>(ui->tbRecoveryTime->value());
    params.iterationCount = static_cast<uint>(ui->tbIterationCount->value());
    params.resultFileName = ui->tbResult->text();

    return params;
}
