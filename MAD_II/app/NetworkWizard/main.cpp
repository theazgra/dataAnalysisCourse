#include "networkwizard.h"
#include <QApplication>
#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{

#pragma omp parallel for
    for (int i = 0; i < 12; ++i) {
        int tid = omp_get_thread_num();
        fprintf(stdout, "Hello World from thread %i\n", tid);
    }

    QApplication a(argc, argv);
    NetworkWizard w;
    w.show();

    return a.exec();
}
