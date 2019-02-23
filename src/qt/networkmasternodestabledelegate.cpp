#include "networkmasternodestabledelegate.h"
#include <QApplication>
#include <QDateTime>
#include <QStyle>
#include <QPalette>
#include <QLinearGradient>
#include <QPixmap>

#include "bitcoinunits.h"
#include "guiconstants.h"
#include "guiutil.h"

#define CORRECTION 20
#define BORDER_RADIUS 10

void NetworkMasternodesTableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    QPixmap bg;

    const int lastColIndex = 3;
    int leftBgMargin = 16;

    if (index.column() == 0) {
        leftBgMargin = 0;
    }
    else if(index.column() == lastColIndex) {
        leftBgMargin = 300;
    }

    if (option.showDecorationSelected && (option.state & QStyle::State_Selected)){
        if (option.state & QStyle::State_Active) {
            painter->fillRect(option.rect, QColor(252, 252, 252));
            bg = QPixmap(":/images/table-element-bg-selected");
        }
    } else {
        bg = QPixmap(":/images/table-element-bg");
    }

    painter->drawPixmap(option.rect.x(), option.rect.y(), bg, leftBgMargin, 0, option.rect.width(), option.rect.height());

    QPen pen(QColor(255, 255, 255), 0);
    painter->setPen(pen);

    painter->setPen(COLOR_BLACK);
    painter->drawText(option.rect.x()+BORDER_RADIUS+CORRECTION, option.rect.y(), option.rect.width(), option.rect.height(), Qt::AlignLeft | Qt::AlignVCenter, index.data().toString());

    painter->restore();
}
