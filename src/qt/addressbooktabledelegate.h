#ifndef ADDRESSBOOKTABLEDELEGATE_H 
#define ADDRESSBOOKTABLEDELEGATE_H 

#include <QStyledItemDelegate>
#include <QPainter>
 
class AddressBookTableDelegate : public QStyledItemDelegate 
{ 
    Q_OBJECT
public: 
    AddressBookTableDelegate() : QStyledItemDelegate() {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
}; 
 
#endif // ADDRESSBOOKTABLEDELEGATE_H
