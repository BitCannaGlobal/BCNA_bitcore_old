#include "transactiontableviewdelegate.h"
#include <QApplication>
#include <QDateTime>
#include <QStyle>
#include <QPalette>
#include <QLinearGradient>

#include "bitcoinunits.h"
#include "guiconstants.h"
#include "guiutil.h"

void TransactionTableViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{

    BaseTableDelegate::paint(painter, option, index);

    if(index.column() == 0) {
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        icon.paint(painter, option.rect.x()+25, option.rect.y()+20, 25, 25);
    }
}
