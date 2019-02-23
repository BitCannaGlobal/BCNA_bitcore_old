#ifndef TRANSACTIONTABLEVIEWDELEGATE_H
#define TRANSACTIONTABLEVIEWDELEGATE_H

#include "basetabledelegate.h"

class TransactionTableViewDelegate : public BaseTableDelegate
{
    Q_OBJECT
public:
    TransactionTableViewDelegate(const int lastColumnIndex = 1, const int lastColumnBgMargin = 300) : BaseTableDelegate(lastColumnIndex, lastColumnBgMargin) {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // TRANSACTIONTABLEVIEWDELEGATE_H
