#ifndef MACSTYLE_H
#define MACSTYLE_H

#include <QMacStyle>
#include <QPlastiqueStyle>

class MacStyle : public QMacStyle
{
    Q_OBJECT
public:
    explicit MacStyle();
    virtual void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *w) const;

signals:

public slots:

};

#endif // MACSTYLE_H
