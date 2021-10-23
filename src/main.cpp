#include <QApplication>
#include <QTranslator>

#include "check_laads_order.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    auto *tran = new QTranslator(&a);
    tran->load(QLocale::system(), "check_laads_order", "_", ":/i1n");
    QApplication::installTranslator(tran);

    check_laads_order w;
    w.show();
//     int b;
    return QApplication::exec();
}