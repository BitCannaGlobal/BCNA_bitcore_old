#ifndef NETWORKMASTERNODESTABLEDELEGATE_H
#define NETWORKMASTERNODESTABLEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class NetworkMasternodesTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    NetworkMasternodesTableDelegate() : QStyledItemDelegate() {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // NETWORKMASTERNODESTABLEDELEGATE_H
