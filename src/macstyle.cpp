#include "macstyle.h"
#include <QStyleOptionToolButton>
#include <QPainter>
#include <QWidget>
#include <QDebug>

MacStyle::MacStyle()
{
}

void MacStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    if(element == CE_ToolButtonLabel)
    {
        QStyleOptionToolButton optTool = *(static_cast<const QStyleOptionToolButton*>(opt));
        optTool.state &= ~QStyle::State_AutoRaise;
        if(optTool.state.testFlag(QStyle::State_MouseOver) &&
           optTool.state.testFlag(QStyle::State_Enabled) &&
           optTool.state.testFlag(QStyle::State_Active) &&
           optTool.state.testFlag(QStyle::State_Raised))
        {
            drawItemPixmap(p, optTool.rect, Qt::AlignCenter, optTool.icon.pixmap(optTool.iconSize.boundedTo(optTool.rect.size()), QIcon::Active) );
        }
        else
        {
            QMacStyle::drawControl(element, opt, p, w);
        }
    }
    else
        QMacStyle::drawControl(element, opt, p, w);


}


QSize MacStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *w) const
{
    if(ct == QStyle::CT_ToolButton)
    {
        return contentsSize;
    }
    return  QMacStyle::sizeFromContents(ct, opt, contentsSize, w);
}
