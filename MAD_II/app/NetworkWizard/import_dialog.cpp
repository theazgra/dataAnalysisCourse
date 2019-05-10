#include "import_dialog.h"
#include "ui_import_dialog.h"

ImportDialog::ImportDialog(QWidget * parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);
    connect(ui->btnSelectpath, &QPushButton::clicked, this, &ImportDialog::select_path_dialog);

}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::select_path_dialog()
{
    QString path = QFileDialog::getOpenFileName(this, "Network file");

    if (path.length() > 0)
    {
        this->ui->tbPath->setText(path);
    }
}

ImportSettings ImportDialog::get_settings() const
{
    ImportSettings settings = {};

    settings.path = this->ui->tbPath->text();
    settings.delimiter = this->ui->tbDelimiter->text();
    settings.preprocess = this->ui->chbPreprocessIds->isChecked();

    return settings;
}
