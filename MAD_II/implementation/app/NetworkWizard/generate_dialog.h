#pragma once

#include <QDialog>
#include <QDebug>
#include <networkLib/network_generator_params.h>

namespace Ui {
class GenerateDialog;
}

class GenerateDialog : public QDialog
{
Q_OBJECT

public:
    explicit GenerateDialog(QWidget * parent = nullptr);
    ~GenerateDialog();
    azgra::networkLib::GeneratorParameters get_generator_params();

private:
    Ui::GenerateDialog * ui;
    azgra::networkLib::NetworkModel selectedModel;
    void invalidate_form_for_selected_model();

private slots:
    void generate_method_changed();
};

