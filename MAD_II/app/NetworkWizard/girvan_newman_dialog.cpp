#include "girvan_newman_dialog.h"
#include "ui_girvan_newman_dialog.h"

GirvanNewmanDialog::GirvanNewmanDialog(QWidget * parent) :
    QDialog(parent),
    ui(new Ui::GirvanNewmanDialog)
{
    ui->setupUi(this);
}

GirvanNewmanDialog::~GirvanNewmanDialog()
{
    delete ui;
}

GirvanNewmanSettings GirvanNewmanDialog::get_settings() const
{
    GirvanNewmanSettings settings = {};

    settings.maxIterationCount = this->ui->tbIterations->value();
    settings.targetModularity = static_cast<float>(this->ui->tbModularity->value());

    return settings;
}
