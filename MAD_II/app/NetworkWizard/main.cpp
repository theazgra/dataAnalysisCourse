#include "network_wizard.h"
#include <QApplication>
#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkWizard w;
    w.show();

    return a.exec();
}
