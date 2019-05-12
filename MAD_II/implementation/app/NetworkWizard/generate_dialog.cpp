#include "generate_dialog.h"
#include "ui_generate_dialog.h"

GenerateDialog::GenerateDialog(QWidget * parent) :
    QDialog(parent),
    ui(new Ui::GenerateDialog)
{
    ui->setupUi(this);

    connect(ui->rbRandom, &QRadioButton::clicked, this, &GenerateDialog::generate_method_changed);
    connect(ui->rbBA, &QRadioButton::clicked, this, &GenerateDialog::generate_method_changed);
    connect(ui->rbBianconi, &QRadioButton::clicked, this, &GenerateDialog::generate_method_changed);
    connect(ui->rbHolmeKim, &QRadioButton::clicked, this, &GenerateDialog::generate_method_changed);
    connect(ui->rbLink, &QRadioButton::clicked, this, &GenerateDialog::generate_method_changed);
    connect(ui->rbCopying, &QRadioButton::clicked, this, &GenerateDialog::generate_method_changed);

    ui->rbRandom->setChecked(true);
    generate_method_changed();
}

GenerateDialog::~GenerateDialog()
{
    delete ui;
}

void GenerateDialog::generate_method_changed()
{
    using namespace azgra::networkLib;
    if (ui->rbRandom->isChecked())
    {
        selectedModel = NetworkModel_Random;
    }
    else if (ui->rbBA->isChecked())
    {
        selectedModel = NetworkModel_BarabasiAlbert;
    }
    else if (ui->rbHolmeKim->isChecked())
    {
        selectedModel = NetworkModel_HolmeKim;
    }
    else if (ui->rbBianconi->isChecked())
    {
        selectedModel = NetworkModel_Bianconi;
    }
    else if (ui->rbLink->isChecked())
    {
        selectedModel = NetworkModel_LinkSelection;
    }
    else if (ui->rbCopying->isChecked())
    {
        selectedModel = NetworkModel_Copying;
    }

    invalidate_form_for_selected_model();
}

void GenerateDialog::invalidate_form_for_selected_model()
{
    bool initSize = false;
    bool finalSize = false;
    bool newEdges = false;
    bool prob = false;
    bool aging = false;
    bool symmetric = false;

    ui->tbProb->setMinimum(0.0);
    ui->tbProb->setMaximum(1.0);

    switch (selectedModel)
    {
        case azgra::networkLib::NetworkModel_Random:
        {
            finalSize = true;
            prob = true;
            symmetric = true;
            ui->lbProbability->setText("Edge probability");
        }
        break;
        case azgra::networkLib::NetworkModel_BarabasiAlbert:
        {
            initSize = true;
            finalSize = true;
            newEdges = true;
            aging = true;
            prob = true;
            ui->lbProbability->setText("Age scaling");
            ui->tbProb->setMinimum(-10.0);
            ui->tbProb->setMaximum(10.0);
        }
        break;
        case azgra::networkLib::NetworkModel_HolmeKim:
        case azgra::networkLib::NetworkModel_Bianconi:
        {
            initSize = true;
            finalSize = true;
            newEdges = true;
            prob = true;
            ui->lbProbability->setText("Choose random neighbor of neighbor probability");
        }
        break;
        case azgra::networkLib::NetworkModel_Copying:
        {
            initSize = true;
            finalSize = true;
            prob = true;
            ui->lbProbability->setText("Copy probability");
        }
        break;
        case azgra::networkLib::NetworkModel_LinkSelection:
        {
            initSize = true;
            finalSize = true;

        }
        break;
        case azgra::networkLib::NetworkModel_None:
            break;

    }
    ui->lbInitialSize->setVisible(initSize);
    ui->tbInitialSize->setVisible(initSize);

    ui->lbFinalSize->setVisible(finalSize);
    ui->tbFinalSize->setVisible(finalSize);

    ui->lbNewEdges->setVisible(newEdges);
    ui->tbNewEdges->setVisible(newEdges);

    ui->lbProbability->setVisible(prob);
    ui->tbProb->setVisible(prob);

    ui->lbAging->setVisible(aging);
    ui->chbAging->setVisible(aging);

    ui->lbSymmetric->setVisible(symmetric);
    ui->chbSymmetric->setVisible(symmetric);

}

azgra::networkLib::GeneratorParameters GenerateDialog::get_generator_params()
{
    using namespace azgra::networkLib;
    GeneratorParameters params = {};
    params.isSet = true;

    params.model = this->selectedModel;
    params.initialSize = static_cast<uint>(ui->tbInitialSize->value());
    params.finalSize = static_cast<uint>(ui->tbFinalSize->value());
    params.newEdgesInStep = static_cast<uint>(ui->tbNewEdges->value());
    params.probability = static_cast<float>(ui->tbProb->value());
    params.ageScaling = params.probability;
    params.aging = ui->chbAging->isChecked();
    params.symmetric = ui->chbSymmetric->isChecked();

    return params;
}
