#include "check_laads_order.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    check_laads_order w;
    w.show();
    return a.exec();
}
