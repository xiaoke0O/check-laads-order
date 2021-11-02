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

  void fill_result_cells(Order *this_order);

  QMap<QString, Order *> orders;

  QColor result_cell_color;
  QString result_cell_text;
 private slots :

  void remove_selected_orders();

  void remove_all_orders();

  void item_selection_changed();

  void search_orders();

  void do_check();

  void show_report();

 private:
  Ui::check_laads_order *ui;
};

#endif  // CHECK_LAADS_ORDER_H
