#ifndef RECEIVECOINSDIALOGTABLEDELEGATE_H
#define RECEIVECOINSDIALOGTABLEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class ReceiveCoinsDialogTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ReceiveCoinsDialogTableDelegate() : QStyledItemDelegate() {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // RECEIVECOINSDIALOGTABLEDELEGATE_H
