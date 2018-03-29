#include <QtGui/QApplication>
#include "lockingui.h"
#include "coutwriter.h"
#include <math.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LockinGui w;
    w.show();

    CoutWriter out(w.newReaderX(), w.newReaderY());
//    out.start();

    return a.exec();
}
