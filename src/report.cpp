//
// Created by xueke on 2021/10/31.
//

#include "report.h"

#include <utility>
#include "ui_report.h"

report::report(QString _order_sn,
			   QStringList _error_files,
			   QStringList _missing_files,
			   QStringList _extra_files) : order_sn(std::move(_order_sn)),
										   error_files(std::move(_error_files)),
										   missing_files(std::move(_missing_files)),
										   extra_files(std::move(_extra_files)),
										   ui(new Ui::report) {
  ui->setupUi(this);
  setWindowTitle(tr("Order %1 Check Report").arg(order_sn));
  set_content();
}

report::~report() {
  delete ui;
}

void report::set_content() {
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
	  foreach(QString text, error_files) ui->textBrowser_error->append(
		  text);
	  foreach(QString text, missing_files) ui->textBrowser_missing->append(
		  text);
	  foreach(QString text, extra_files) ui->textBrowser_unwanted->append(
		  text);

}


