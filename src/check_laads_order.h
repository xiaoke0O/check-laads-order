#ifndef CHECK_LAADS_ORDER_H
#define CHECK_LAADS_ORDER_H

#include <QMainWindow>
#include <QDir>
#include "Order.h"


QT_BEGIN_NAMESPACE
namespace Ui {
    class check_laads_order;
}
QT_END_NAMESPACE

class check_laads_order : public QMainWindow {
Q_OBJECT

public:
    explicit check_laads_order(QWidget *parent = nullptr);

    ~check_laads_order() override;

private:
    QString get_orders_directory();

    QMap<QString, Order*> orders;
private slots :

    void search_orders();

    void do_check();

private:
    Ui::check_laads_order *ui;
};

#endif  // CHECK_LAADS_ORDER_H
