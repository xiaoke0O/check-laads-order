//
// Created by xueke on 2021/10/31.
//

#ifndef CHECK_LAADS_ORDER_REPORT_H
#define CHECK_LAADS_ORDER_REPORT_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
    class report;
}
QT_END_NAMESPACE

class report : public QWidget {
Q_OBJECT
public:
    report(QString order_sn);

    void set_content(QStringList &error_files,QStringList &missing_files,QStringList &extra_files);

    ~report() override;

private:
    Ui::report *ui;
};


#endif //CHECK_LAADS_ORDER_REPORT_H
