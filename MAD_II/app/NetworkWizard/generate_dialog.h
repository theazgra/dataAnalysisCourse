#pragma once

#include <QDialog>

namespace Ui {
class GenerateDialog;
}

class GenerateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateDialog(QWidget *parent = nullptr);
    ~GenerateDialog();

private:
    Ui::GenerateDialog *ui;
};

