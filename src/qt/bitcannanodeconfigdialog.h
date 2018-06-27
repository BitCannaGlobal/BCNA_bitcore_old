#ifndef BCNANODECONFIGDIALOG_H
#define BCNANODECONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class BitCannaNodeConfigDialog;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Dialog showing transaction details. */
class BitCannaNodeConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BitCannaNodeConfigDialog(QWidget *parent = 0, QString nodeAddress = "123.456.789.123:28666", QString privkey="MASTERNODEPRIVKEY");
    ~BitCannaNodeConfigDialog();

private:
    Ui::BitCannaNodeConfigDialog *ui;
};

#endif // BCNANODECONFIGDIALOG_H
