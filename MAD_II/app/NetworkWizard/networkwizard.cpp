#include "networkwizard.h"
#include "ui_networkwizard.h"

NetworkWizard::NetworkWizard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NetworkWizard)
{
    ui->setupUi(this);
}

NetworkWizard::~NetworkWizard()
{
    delete ui;
}
