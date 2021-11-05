//
// Created by Xueke on 2021/10/29.
//
#include "Order.h"
#include "ui_report.h"

#include <cinttypes>

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <utility>

#include "fast_cksum.h"

Order::Order(QString local_order_dir, const QString &checksum_file)
	: order_dir(std::move(local_order_dir)),
	  calculate_status(true),
	  ui_report(new Ui::report) {
  ui_report->setupUi(this);
  order_sn = checksum_file.split("_").takeLast();
  parsing_checksum_file(checksum_file);
  parsing_local_file();
}

Order::~Order() {
  delete ui_report;
}
void Order::parsing_checksum_file(const QString &cksum_file) {
  QFile infile(cksum_file);
  if (infile.open(QIODevice::ReadOnly)) {
	QString line;
	QTextStream in(&infile);
	while (!in.atEnd()) {
	  line = in.readLine();
	  if (line.startsWith('#'))
		continue;
	  QStringList parts = line.split(' ', Qt::SkipEmptyParts);
	  QString checksum = parts[0];
	  QString file_name = parts[2];
	  order_files_package.insert(file_name, checksum);
	}

  }
  infile.close();
}

void Order::parsing_local_file() {
  QStringList filter{"*.hdf", "*.nc"};
  QDirIterator order_file_iter(order_dir, filter, QDir::Files);
  while (order_file_iter.hasNext()) {
	local_files_list << order_file_iter.next();
  }
}

uint32_t Order::get_file_cksum(FILE *fp) {

  const uint64_t buf_size = 64 << 10;
  void *buffer = nullptr;
  int ret = posix_memalign(&buffer, 65536, buf_size);
  if (ret != 0 || buffer == nullptr) {
	fprintf(stderr, "Failed to allocate %"
					PRIu64
					" bytes\n", buf_size);
	exit(1);
  }

  uint32_t partial_crc = CRC32_FAST_SEED;
  size_t total_size = 0;
  size_t count;
  while ((count = fread(buffer, 1, buf_size, fp))) {
	partial_crc = crc32_fast_partial(buffer, count, partial_crc);
	total_size += count;
  }

  uint32_t crc = crc32_fast_finalize(total_size, partial_crc);
#ifdef Q_OS_WINDOWS
  _aligned_free(buffer);
#else
  free(buffer);
#endif  // Q_OS_WINDOWS
  return crc;
}

bool Order::calculate_local_cksum() {
  auto *progressDialog = new QProgressDialog;
  progressDialog->setCancelButtonText(tr("&Cancel"));
  int file_count = local_files_list.size();
  progressDialog->setRange(0, file_count);
  progressDialog->setWindowTitle(tr("Calculate Order Files cksum"));

  for (decltype(local_files_list.size()) i = 0;
	   i < local_files_list.size(); i++) {
	QString file_name = local_files_list[i].split("/").takeLast();
	const char *file_path_c = local_files_list[i].toLocal8Bit().data();
	FILE *fp;
	fp = fopen(file_path_c, "rb");
	if(!fp) {
	  QMessageBox::critical(this, "Open Error!", "File opening failed");
	 calculate_status= false;
	  break;
	}
	QString checksum = QString::number(get_file_cksum(fp));
	fclose(fp);
	local_files_package.insert(file_name, checksum);

	progressDialog->setValue(i);
	progressDialog->setLabelText(
		tr("Running file number %1 of %n", nullptr,
		   local_files_list.size()).arg(i));
	QCoreApplication::processEvents();
	if (progressDialog->wasCanceled()) {
	  calculate_status = false;
	  break;
	}
  }
  progressDialog->setValue(file_count);
  if (calculate_status)compare_cksum();
  return calculate_status;
}

void Order::compare_cksum() {
  auto order_copy = order_files_package;
  auto local_copy = local_files_package;
  QMapIterator<QString, QString> i(order_copy);
  QMapIterator<QString, QString> i_local(local_copy);
  while (i_local.hasNext()) {
	i_local.next();
	auto x = order_copy.find(i_local.key());
	//说明找到了并且匹配了
	if (x != order_copy.end() && x.value() == i_local.value()) {
	  match_files << x.key();
	  order_copy.erase(x);
	  continue;
	}
	//说明找到了但是不匹配
	if (x != order_copy.end() && x.value() != i_local.value()) {
	  error_files << x.key();
	  order_copy.erase(x);
	  continue;
	}
	//说明在b中有而a中没有，那么这个就是多余项
	if (x == order_copy.end())
	  extra_files << i_local.key();
  }
  //等全都检索完了，a也擦除完了，a中剩下的没被擦除的，就是b中缺失的。
  missing_files << order_copy.keys();

  order_copy.clear();
  local_copy.clear();
}

bool Order::get_check_result() {
  return (!error_files.isEmpty() ||
	  !missing_files.isEmpty() ||
	  !extra_files.isEmpty());
}

void Order::show_report() {

  setWindowTitle(tr("Order %1 Check Report").arg(order_sn));
  ui_report->pushButton_creat->setDisabled(true);
  ui_report->pushButton_delete->setDisabled(true);

  fill_report();

  connect(ui_report->pushButton_creat,
		  &QPushButton::clicked,
		  this,
		  &Order::create_downloadable_files_link);
  connect(ui_report->pushButton_delete,
		  &QPushButton::clicked,
		  this,
		  &Order::delete_extra_files);
  this->show();
}

void Order::fill_report() {
  // 添上文件个数
  ui_report->label_error->setText(
	  ui_report->label_error->text() +
		  QString("(%1)").arg(QString::number(error_files.size())));
  ui_report->label_missing->setText(
	  ui_report->label_missing->text() +
		  QString("(%1)").arg(QString::number(missing_files.size())));
  ui_report->label_unwanted->setText(
	  ui_report->label_unwanted->text() +
		  QString("(%1)").arg(QString::number(extra_files.size())));

  // 文件内容
  if (!error_files.isEmpty() || !missing_files.isEmpty()) {
	for (auto &text: error_files)
	  ui_report->textBrowser_error->append(text);
	for (auto &text: missing_files)
	  ui_report->textBrowser_missing->append(text);
	ui_report->pushButton_creat->setEnabled(true);
  }
  if (!extra_files.isEmpty()) {
	for (auto &text: extra_files)
	  ui_report->textBrowser_unwanted->append(text);
	ui_report->pushButton_delete->setEnabled(true);
  }
}

void Order::create_downloadable_files_link() {
  QStringList links;
  QString website = "https://ladsweb.modaps.eosdis.nasa.gov/archive/orders";
  for (auto &file_name: error_files)
	links << QString("%1/%2/%3").arg(website, order_sn, file_name);
  for (auto &file_name: missing_files)
	links << QString("%1/%2/%3").arg(website, order_sn, file_name);

  // 文件保存框
  QString save_file_file = QString("%1/more_download_file_links.txt").arg(order_dir);
  QString file_name = QFileDialog::getSaveFileName(this,
												   tr("Save Download Links"),
												   save_file_file,
												   tr("Text (*.txt)"));
  if(file_name.isEmpty())
	return;
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
void Order::delete_extra_files() {
  QStringList extra_files_path;
  for (auto &file: extra_files) {
	extra_files_path << order_dir + "/" + file;
  }
  QMessageBox::StandardButton rb = QMessageBox::question(this,
														 tr("Delete Files"),
														 tr("Are you sure you "
															"want to delete these files?"),
														 QMessageBox::Yes
															 | QMessageBox::No,
														 QMessageBox::No);
  int i = 0;
  if (rb == QMessageBox::Yes)
	for (auto &file: extra_files_path)
	  if (QFile(file).remove())
		i++;
  QMessageBox::information(this,
						   tr("Delete Result"),
						   tr("Successfully Delete %1 file(s)").arg(i));
  ui_report->textBrowser_unwanted->clear();
  ui_report->pushButton_delete->setDisabled(true);
}
