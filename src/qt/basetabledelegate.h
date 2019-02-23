#ifndef BASETABLEDELEGATE_H
#define BASETABLEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class BaseTableDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    BaseTableDelegate(const int lastColumnIndex = 1, const int lastColumnBgMargin = 300) : QStyledItemDelegate() {
        this->lastColumnIndex = lastColumnIndex;
        this->lastColumnBgMargin = lastColumnBgMargin;
    }
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
protected:
    int lastColumnIndex;
    int lastColumnBgMargin;
};

#endif // BASETABLEDELEGATE_H
