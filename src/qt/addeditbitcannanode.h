#ifndef ADDEDITBCNANODE_H
#define ADDEDITBCNANODE_H

#include <QDialog>

namespace Ui {
class AddEditBitCannaNode;
}


class AddEditBitCannaNode : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditBitCannaNode(QWidget *parent = 0);
    ~AddEditBitCannaNode();

protected:

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

signals:

private:
    Ui::AddEditBitCannaNode *ui;
};

#endif // ADDEDITBCNANODE_H
