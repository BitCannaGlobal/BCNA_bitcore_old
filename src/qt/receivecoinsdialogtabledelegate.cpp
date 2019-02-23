#include "receivecoinsdialogtabledelegate.h"
#include <QApplication>
#include <QDateTime>
#include <QStyle>
#include <QPalette>
#include <QLinearGradient>

#include "bitcoinunits.h"
#include "guiconstants.h"
#include "guiutil.h"

#define ROW_HEIGHT 60
#define CORRECTION 20
#define BORDER_RADIUS 10

void ReceiveCoinsDialogTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{

    painter->save();

    QFont font = painter->font();

    painter->setRenderHint(QPainter::Antialiasing);

    QPainterPath path;

    if (index.column() == 0) {
        path.addRoundedRect(option.rect.x()+5, option.rect.y(), option.rect.width(), ROW_HEIGHT, BORDER_RADIUS, BORDER_RADIUS);
    }
    else if(index.column() == 1) {
        path.addRoundedRect(option.rect.x()-BORDER_RADIUS, option.rect.y(), option.rect.width(), ROW_HEIGHT, BORDER_RADIUS, BORDER_RADIUS);
    }

    QPen pen(QColor(255, 255, 255), 0);
    painter->setPen(pen);
    painter->fillPath(path, QColor(255, 255, 255));
    painter->drawPath(path);

    painter->setPen(COLOR_BLACK);
    painter->drawText(option.rect.x()+BORDER_RADIUS+CORRECTION, option.rect.y(), option.rect.width(), option.rect.height(), Qt::AlignLeft | Qt::AlignVCenter, index.data().toString());

    painter->restore();
}
