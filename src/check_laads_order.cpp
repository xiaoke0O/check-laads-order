#include "check_laads_order.h"
#include "./ui_check_laads_order.h"
#include "fast_cksum.h"
#include<QAction>
#include <inttypes.h>
#include <QDebug>
check_laads_order::check_laads_order(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::check_laads_order)
{
    ui->setupUi(this);
    connect(ui->actionAbout_Qt,&QAction::triggered,qApp,&QApplication::aboutQt);
//    const char *filename = "/home/zheng/workspace/hellosome/testorder/MOD13A3.A2018182.mosaic.006.2021290053459.psmcrp_000501663056.1_km_monthly_NDVI-1_km_monthly_NDVI.tif";
//    FILE *fp;
//    fp = fopen(filename, "rb");
//    if (fp == NULL)
//    {
//        fprintf(stderr, "File %s not found or cannot be opened.\n", filename);
//        exit(1);
//    }
//    qDebug() << get_file_cksum(fp);
}
uint32_t check_laads_order::get_file_cksum(FILE *fp)
{
    const uint64_t BUFSIZE = 64 << 10;
    void *buffer = NULL;
    int ret = posix_memalign(&buffer, 65536, BUFSIZE);
    if (ret != 0 || buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate %" PRIu64 " bytes\n", BUFSIZE);
        exit(1);
    }

    uint32_t partial_crc = CRC32_FAST_SEED;
    size_t totalsize = 0;
    size_t count;
    while ((count = fread(buffer, 1, BUFSIZE, fp)))
    {
        partial_crc = crc32_fast_partial(buffer, count, partial_crc);
        totalsize += count;
    }

    uint32_t crc = crc32_fast_finalize(totalsize, partial_crc);
    free(buffer);
    return crc;
}

check_laads_order::~check_laads_order()
{
    delete ui;
}

