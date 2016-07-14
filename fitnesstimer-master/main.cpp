#include "fitnesstimer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FitnessTimer w;
    w.show();

    return a.exec();
}
