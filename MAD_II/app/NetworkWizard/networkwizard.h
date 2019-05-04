#ifndef NETWORKWIZARD_H
#define NETWORKWIZARD_H

#include <QMainWindow>

namespace Ui {
class NetworkWizard;
}

class NetworkWizard : public QMainWindow
{
    Q_OBJECT

public:
    explicit NetworkWizard(QWidget *parent = nullptr);
    ~NetworkWizard();

private:
    Ui::NetworkWizard *ui;
};

#endif // NETWORKWIZARD_H
