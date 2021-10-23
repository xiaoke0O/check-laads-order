#ifndef CHECK_LAADS_ORDER_H
#define CHECK_LAADS_ORDER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class check_laads_order;
}
QT_END_NAMESPACE

class check_laads_order : public QMainWindow {
Q_OBJECT

public:
    check_laads_order(QWidget *parent = nullptr);

    ~check_laads_order();

private:
    uint32_t get_file_cksum(FILE *fp);

    QString get_orders_directory();

    void search_orders();

private:
    Ui::check_laads_order *ui;
};

#endif  // CHECK_LAADS_ORDER_H
