#include "check_laads_order.h"
#include "./ui_check_laads_order.h"
#include "fast_cksum.h"
#include <QAction>
#include <QDebug>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


check_laads_order::check_laads_order(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::check_laads_order)
{
    ui->setupUi(this);
    connect(ui->actionAbout_Qt,&QAction::triggered,qApp,&QApplication::aboutQt);
#ifdef __MINGW32__
    const char *filename = QString("C:\\Users\\xueke\\Pictures\\微信图片_20210918235156.jpg").toLocal8Bit();
#else
        const char *filename = "C:\\Users\\xueke\\Pictures\\微信图片_20210918235156.jpg";
#endif
    FILE *fp;
#ifdef _MSC_VER
    if(fopen_s(&fp,filename,"rb"))
#else
    fp=fopen(filename, "rb");
    if (!fp)
#endif
    {
        fprintf(stderr, "File %s not found or cannot be opened.\n", filename);
        exit(1);
    }
    qDebug() << get_file_cksum(fp);
    fclose(fp);
}

uint32_t check_laads_order::get_file_cksum(FILE *fp)
{
#ifdef Q_OS_WINDOWS
#define posix_memalign(p, a, s)  (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)
#endif
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
#ifdef Q_OS_WINDOWS
    _aligned_free(buffer);
#else
    free(buffer);
#  endif //Q_OS_WINDOWS
    return crc;
}

check_laads_order::~check_laads_order()
{
    delete ui;
}

