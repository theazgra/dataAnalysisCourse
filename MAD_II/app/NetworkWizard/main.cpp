#include "networkwizard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkWizard w;
    w.show();

    return a.exec();
}
