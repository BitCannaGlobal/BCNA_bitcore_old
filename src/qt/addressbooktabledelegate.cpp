#include "addressbooktabledelegate.h"
#include <QApplication>
void AddressBookTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();

    QFont font = painter->font();

    painter->setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
//        path.addRoundedRect(option.rect.x()-ICON_OFFSET, option.rect.y()-CORRECTION, option.rect.width()-1, option.rect.height()+(CORRECTION * 2.5), BORDER_RADIUS, BORDER_RADIUS);
    QPen pen(QColor(0, 0, 0), 0);
    painter->setPen(pen);
    painter->fillPath(path, QColor(0, 0, 0));
    painter->drawPath(path);

    QRect mainRect = option.rect;
    mainRect.moveLeft(1);

    int xspace = 8;
    int ypad = 6;
    int halfheight = (mainRect.height() - 2 * ypad) / 2;
    QRect amountRect(mainRect.left() + xspace, mainRect.top() + ypad + halfheight, mainRect.width() - xspace, halfheight);

    if (index.column() == 1) {
        painter->drawText(amountRect, Qt::AlignRight | Qt::AlignVCenter, "test");
    }
    else
        QStyledItemDelegate::paint(painter, option, index);

    painter->restore();
}
