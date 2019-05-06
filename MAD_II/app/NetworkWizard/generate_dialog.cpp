#include "generate_dialog.h"
#include "ui_generate_dialog.h"

GenerateDialog::GenerateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenerateDialog)
{
    ui->setupUi(this);
}

GenerateDialog::~GenerateDialog()
{
    delete ui;
}
