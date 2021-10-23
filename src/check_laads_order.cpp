#include "check_laads_order.h"

#include <inttypes.h>

#include <QFileDialog>
#include <QStandardPaths>

#include "./ui_check_laads_order.h"
#include "fast_cksum.h"

check_laads_order::check_laads_order(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::check_laads_order) {
    ui->setupUi(this);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp,
            &QApplication::aboutQt);
    connect(ui->actionCkeck_Order, &QAction::triggered, this,
            &check_laads_order::search_orders);
}

uint32_t check_laads_order::get_file_cksum(FILE *fp) {
#ifdef Q_OS_WINDOWS
#define posix_memalign(p, a, s) \
  (((*(p)) = _aligned_malloc((s), (a))), *(p) ? 0 : errno)
#endif
    const uint64_t BUFSIZE = 64 << 10;
    void *buffer = NULL;
    int ret = posix_memalign(&buffer, 65536, BUFSIZE);
    if (ret != 0 || buffer == NULL) {
        fprintf(stderr, "Failed to allocate %" PRIu64 " bytes\n", BUFSIZE);
        exit(1);
    }

    uint32_t partial_crc = CRC32_FAST_SEED;
    size_t totalsize = 0;
    size_t count;
    while ((count = fread(buffer, 1, BUFSIZE, fp))) {
        partial_crc = crc32_fast_partial(buffer, count, partial_crc);
        totalsize += count;
    }

    uint32_t crc = crc32_fast_finalize(totalsize, partial_crc);
#ifdef Q_OS_WINDOWS
    _aligned_free(buffer);
#else
    free(buffer);
#endif  // Q_OS_WINDOWS
    return crc;
}

QString check_laads_order::get_orders_directory() {
    QString dialog_title = tr("Open Orders Directory");
    QString open_dir =
            QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    return QFileDialog::getExistingDirectory(this, dialog_title, open_dir);
}

void check_laads_order::search_orders() {}

check_laads_order::~check_laads_order() { delete ui; }
