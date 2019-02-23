#ifndef EDITMASTERNODEDIALOG_H
#define EDITMASTERNODEDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
    class EditMasterNodeDialog;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

class EditMasterNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditMasterNodeDialog(QWidget *parent = 0, QString alias = "", QString address = "", QString privateKey = "", QString txHash = "");
    ~EditMasterNodeDialog();
    std::string newIp;
    std::string index;
    void done(int);

private:
    Ui::EditMasterNodeDialog *ui;
    std::string sAddress;
};

#endif // EDITMASTERNODEDIALOG_H
