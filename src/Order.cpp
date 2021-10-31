//
// Created by Xueke on 2021/10/29.
//
#include "Order.h"

#include <cinttypes>

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QCoreApplication>
//TODO: Release时删除
#include <QDebug>


#include "fast_cksum.h"

Order::Order(QString order_dir, QString checksum_file) {
    order_sn = checksum_file.split("_").takeLast();
    parsing_checksum_file(checksum_file);
    parsing_local_file(order_dir);
}

void Order::parsing_checksum_file(QString cksum_file) {
    QFile infile(cksum_file);
    if (infile.open(QIODevice::ReadOnly)) {
        QString line;
        QTextStream in(&infile);
        while (!in.atEnd()) {
            line = in.readLine();
            if (line.startsWith('#'))
                continue;
            QStringList parts = line.split(' ', QString::SkipEmptyParts);
            QString checksum = parts[0];
            QString file_name = parts[2];
            order_files_package.insert(file_name, checksum);
        }
//        qDebug() << order_files;
    }
}

void Order::parsing_local_file(QString local_order_dir) {
    QStringList filter{"*.hdf", "*.nc"};
    QDirIterator order_file_iter(local_order_dir, filter, QDir::Files);
    while (order_file_iter.hasNext()) {
        local_files_list << order_file_iter.next();
    }
}

uint32_t Order::get_file_cksum(FILE *fp) {

    const uint64_t BUFSIZE = 64 << 10;
    void *buffer = NULL;
    int ret = posix_memalign(&buffer, 65536, BUFSIZE);
    if (ret != 0 || buffer == NULL) {
        fprintf(stderr, "Failed to allocate %"
                        PRIu64
                        " bytes\n", BUFSIZE);
        exit(1);
    }

    uint32_t partial_crc = CRC32_FAST_SEED;
    size_t total_size = 0;
    size_t count;
    while ((count = fread(buffer, 1, BUFSIZE, fp))) {
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

void Order::calculate_local_cksum() {
    QProgressDialog progressDialog;
    progressDialog.setCancelButtonText(tr("&Cancel"));
    int file_count = local_files_list.size();
    qDebug() << file_count;
    progressDialog.setRange(0, file_count);
    progressDialog.setWindowTitle(tr("Calculate Order Files cksum"));

    for (decltype(local_files_list.size()) i = 0;
         i < local_files_list.size(); i++) {
        QString file_name = local_files_list[i].split("/").takeLast();
        std::string str = local_files_list[i].toStdString();
        const char *file_path_c = str.c_str();
        FILE *fp;
        fp = fopen(file_path_c, "rb");
        QString checksum = QString::number(get_file_cksum(fp));
        fclose(fp);
        local_files_package.insert(file_name, checksum);

        progressDialog.setValue(i);
        progressDialog.setLabelText(
                tr("Running file number %1 of %n", nullptr,
                   local_files_list.size()).arg(i));
        QCoreApplication::processEvents();
        if (progressDialog.wasCanceled())
            break;
    }
    progressDialog.setValue(file_count);

    compare_cksum();
}

void Order::compare_cksum() {
    QMapIterator<QString, QString> i(order_files_package);
    QMapIterator<QString, QString> i_local(local_files_package);
    while (i_local.hasNext()) {
        i_local.next();
        auto x = order_files_package.find(i_local.key());
        //说明找到了并且匹配了
        if (x != order_files_package.end() && x.value() == i_local.value()) {
            match_files << x.key();
            order_files_package.erase(x);
            continue;
        }
        //说明找到了但是不匹配
        if (x != order_files_package.end() && x.value() != i_local.value()) {
            error_files << x.key();
            order_files_package.erase(x);
            continue;
        }
        //说明在b中有而a中没有，那么这个就是多余项
        if (x == order_files_package.end())
            extra_files << i_local.key();
    }
    //等全都检索完了，a也擦除完了，a中剩下的没被擦除的，就是b中缺失的。
    missing_files << order_files_package.keys();

    qDebug() << tr("ab匹配的：%1").arg(match_files.size());
    qDebug() << tr("b中错误的：%1").arg(error_files.size());
    qDebug() << tr("b中缺失的：%1").arg(missing_files.size());
    qDebug() << tr("b中多余的：%1").arg(extra_files.size());
}
