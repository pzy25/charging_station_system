#include "centeraligneddelegate.h"
#include <QStyleOptionViewItem>
#include <QPainter>

void CenterAlignedDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                  const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::AlignCenter;  // 设置文字居中对齐
    QStyledItemDelegate::paint(painter, opt, index);
}
