#include "check_laads_order.h"


#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QDirIterator>
#include <QStringList>
#include <QProgressDialog>

#include "./ui_check_laads_order.h"
#include "fast_cksum.h"

check_laads_order::check_laads_order(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::check_laads_order) {
    ui->setupUi(this);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp,
            &QApplication::aboutQt);
    connect(ui->actionImport_Order, &QAction::triggered, this,
            &check_laads_order::search_orders);
    connect(ui->actionStart, &QAction::triggered, this,
            &check_laads_order::do_check);
}

QString check_laads_order::get_orders_directory() {
    QString dialog_title = tr("Open Orders Directory");
    //todo: release时改回来
//    QString open_dir = QStandardPaths::writableLocation(
//            QStandardPaths::DownloadLocation);
    QString open_dir = "D:\\Datasets\\Remote_Sensing";
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

        orders.insert(it.filePath(),
                      Order(it.fileInfo().canonicalPath(), it.filePath()));
    }
    if (!orders.isEmpty()) {
        ui->actionStart->setEnabled(true);
        ui->tableWidget->setRowCount(orders.size());
        int ix = 0;
        for (auto &w: orders) {
            ui->tableWidget->setItem(ix, 0, new QTableWidgetItem(
                    w.get_order_sn()));
            ui->tableWidget->setItem(ix, 1, new QTableWidgetItem(
                    QString::number(w.get_order_file_number())));
            ui->tableWidget->setItem(ix, 2, new QTableWidgetItem(
                    QString::number(w.get_local_file_number())));
            ix++;
        }
    }
}


check_laads_order::~check_laads_order() { delete ui; }

void check_laads_order::do_check() {
    qDebug()<<"Begin Check";
//    for (auto &w:orders) {
//        for (:) {
//
//        }
//    }
//    QProgressDialog progressDialog(this);
//    progressDialog.setCancelButtonText(tr("&Cancel"));
//    int file_count=ui->tableWidget->item(0,1)->text().toInt();
//    qDebug()<<file_count;
//    progressDialog.setRange(0, file_count);
//    progressDialog.setWindowTitle(tr("Find Files"));
//    for (int i = 0; i < file_count; i++) {
//        progressDialog.setValue(i);
//        QCoreApplication::processEvents();
//        if (progressDialog.wasCanceled())
//            break;
//    }
//    progressDialog.setValue(file_count);
}


