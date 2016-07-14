#include <QCoreApplication>
#include "../partition.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Partition *part = Partition::create("Fetal");
    part->setFileInfo("/root/fetal.bin", "1.1", "1.0", false, 0);
    part->write();
    
    return a.exec();
}
