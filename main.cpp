#include "lscpedit.h"

#include <QApplication>
//test
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    lscpedit w;
    w.show();
    return a.exec();
}
