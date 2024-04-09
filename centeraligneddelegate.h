#ifndef CENTERALIGNEDDELEGATE_H
#define CENTERALIGNEDDELEGATE_H

#include <QStyledItemDelegate>

class CenterAlignedDelegate : public QStyledItemDelegate
{
public:
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
};

#endif // CENTERALIGNEDDELEGATE_H
