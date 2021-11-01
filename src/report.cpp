//
// Created by xueke on 2021/10/31.
//

#include "report.h"

#include <utility>
#include "ui_report.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

report::report(QString _order_dir,
			   QString _order_sn,
			   QStringList _error_files,
			   QStringList _missing_files,
			   QStringList _extra_files) : order_dir(std::move(_order_dir)),
										   order_sn(std::move(_order_sn)),
										   error_files(std::move(_error_files)),
										   missing_files(std::move(_missing_files)),
										   extra_files(std::move(_extra_files)),
										   ui(new Ui::report) {
  ui->setupUi(this);
  setWindowTitle(tr("Order %1 Check Report").arg(order_sn));
  ui->pushButton_creat->setDisabled(true);
  ui->pushButton_delete->setDisabled(true);
  set_content();

  connect(ui->pushButton_creat, &QPushButton::clicked,
		  this, &report::create_downloadable_files_link);
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
  if (!error_files.isEmpty() || !missing_files.isEmpty()) {
	for (auto &text: error_files)
	  ui->textBrowser_error->append(text);
	for (auto &text: missing_files)
	  ui->textBrowser_missing->append(text);
	ui->pushButton_creat->setEnabled(true);
  }
  if (!extra_files.isEmpty()) {
	for (auto &text: extra_files)
	  ui->textBrowser_unwanted->append(text);
	ui->pushButton_delete->setEnabled(true);
  }
}
void report::create_downloadable_files_link() {
  QStringList links;
  QString website = "https://ladsweb.modaps.eosdis.nasa.gov/archive/orders";
  for (auto &file_name: error_files)
	links << QString("%1/%2/%3").arg(website, order_sn, file_name);
  for (auto &file_name: missing_files)
	links << QString("%1/%2/%3").arg(website, order_sn, file_name);

  // 文件保存框
  QString save_file_file = QString("%1/redownload_file_links.txt").arg(order_dir);
  QString file_name = QFileDialog::getSaveFileName(this,
												   tr("Save Download Links"),
												   save_file_file,
												   tr("Text (*.txt)"));
  QFile write_out(file_name);
  if (write_out.open(QFile::WriteOnly | QFile::Text)) {
	QTextStream s(&write_out);
	s << links.join("\n");
	QMessageBox::information(this, tr("Successfully Save!"),
							 tr("The download links for the error files and "
								"missing files have been saved, please download them "
								"as soon as possible <b>within the validity period of "
								"the order</b>."));
  } else {
	QMessageBox::critical(this, tr("Save Error"), tr("Error writing this file"));
  }
  write_out.close();
}


