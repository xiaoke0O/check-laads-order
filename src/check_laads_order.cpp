#include "check_laads_order.h"


#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QDirIterator>
#include <QStringList>
#include <QElapsedTimer>

#include "./ui_check_laads_order.h"
#include "fast_cksum.h"

check_laads_order::check_laads_order(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::check_laads_order) {
    ui->setupUi(this);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp,
            &QApplication::aboutQt);
    connect(ui->actionImport_Order, &QAction::triggered, this,
            &check_laads_order::search_orders);
    connect(ui->actionClear_All, &QAction::triggered, this,
            &check_laads_order::remove_all_orders);
    connect(ui->actionStart, &QAction::triggered, this,
            &check_laads_order::do_check);
    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this,
            &check_laads_order::item_selection_changed);
    connect(ui->actionClear_Selected, &QAction::triggered, this,
            &check_laads_order::remove_selected_orders);
}

QString check_laads_order::get_orders_directory() {
    QString dialog_title = tr("Open Orders Directory");
    //todo: release时改回来
//    QString open_dir = QStandardPaths::writableLocation(
//            QStandardPaths::DownloadLocation);
    QString open_dir = "/media/xueke/bigDatabase/Datasets/Remote_Sensing";
    return QFileDialog::getExistingDirectory(this, dialog_title, open_dir);
}

void check_laads_order::search_orders() {
    QString order_dir = get_orders_directory();
    if (order_dir.isEmpty())
        return;
    QStringList filter{"checksums_*"};
    QDirIterator it(order_dir, filter, QDir::NoFilter,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        auto *_order = new Order(it.fileInfo().canonicalPath(), it.filePath());
        orders.insert(_order->get_order_sn(), _order);
    }
    if (!orders.isEmpty()) {
        ui->tableWidget->setRowCount(orders.size());
        int ix = 0;
        for (auto &w: orders) {
            ui->tableWidget->setItem(ix, 0, new QTableWidgetItem(
                    w->get_order_sn()));
            ui->tableWidget->setItem(ix, 1, new QTableWidgetItem(
                    QString::number(w->get_order_file_number())));
            ui->tableWidget->setItem(ix, 2, new QTableWidgetItem(
                    QString::number(w->get_local_file_number())));
            ix++;
        }
        ui->actionStart->setEnabled(true);
        ui->actionClear_All->setEnabled(true);
    }
}


check_laads_order::~check_laads_order() { delete ui; }

void check_laads_order::do_check() {
    QElapsedTimer timer;
    for (auto &w: orders) {
        timer.start();
        w->calculate_local_cksum();
        qDebug() << timer.elapsed();
    }
}

void check_laads_order::remove_all_orders() {
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->actionStart->setEnabled(false);
    ui->actionClear_All->setEnabled(false);

    orders.clear();
}

void check_laads_order::remove_selected_orders() {
    auto indexes = ui->tableWidget->selectionModel()->selectedRows();
    if (indexes.size() == orders.size()) {
        remove_all_orders();
    } else {
        for (auto &a: indexes) {
            auto sn = ui->tableWidget->item(a.row(), 0)->text();
            orders.erase(orders.find(sn));
            ui->tableWidget->removeRow(a.row());
        }
    }
}

void check_laads_order::item_selection_changed() {
    bool b = ui->tableWidget->selectedItems().isEmpty();
    ui->actionClear_Selected->setEnabled(!b);
}


