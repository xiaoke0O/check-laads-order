//
// Created by xueke on 2021/10/31.
//

#include "report.h"
#include "ui_report.h"

report::report(QString order_sn) : ui(new Ui::report) {
    ui->setupUi(this);
    setWindowTitle(tr("Order %1 Check Report").arg(order_sn));
}

report::~report() {
    delete ui;
}

void report::set_content(QStringList &error_files, QStringList &missing_files,
                         QStringList &extra_files) {
    // 添上文件个数
    ui->label_error->setText(
            ui->label_error->text() +
            QString("(%1)").arg(QString::number(error_files.size())));
    ui->label_missing->setText(
            ui->label_missing->text() +
            QString("(%1)").arg(QString::number(missing_files.size())));
    ui->label_unwanted->setText(
            ui->label_unwanted->text() +
            QString("(%1)").arg(QString::number(extra_files.size())));

    // 文件内容
            foreach(QString text, error_files)
            ui->textBrowser_error->append(
                    text);
            foreach(QString text, missing_files)
            ui->textBrowser_missing->append(
                    text);
            foreach(QString text, extra_files)
            ui->textBrowser_unwanted->append(
                    text);

}


