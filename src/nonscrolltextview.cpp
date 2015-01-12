#include "nonscrolltextview.h"
#include <qmath.h>
#include <QPainter>
#include <QEvent>
#include <QDebug>
#include <QTimer>

#define MARGIN 5

NonScrollTextView::NonScrollTextView(QWidget *parent) :
    QWidget(parent)
{    

}

NonScrollTextView::~NonScrollTextView()
{

}

void NonScrollTextView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    p.setClipRect(MARGIN, MARGIN, width() - 2*MARGIN, height() - 2*MARGIN);
    p.translate(MARGIN, height() - totalLineHeight* allowedEntries - MARGIN);
    p.drawPixmap(0, 0, textPixmap, 0, totalLineHeight*pixmapCurrentLine, width(), textPixmap.height() - totalLineHeight*pixmapCurrentLine  );
    p.drawPixmap(0, textPixmap.height() - totalLineHeight*pixmapCurrentLine, textPixmap, 0, 0, width(), totalLineHeight*pixmapCurrentLine  );
    p.end();
}

void NonScrollTextView::resizeEvent(QResizeEvent *)
{
    updateParameters();
}

void NonScrollTextView::appendLine(QString text)
{
    contents.prepend(text);

    QPainter p(&textPixmap);
    p.setPen(Qt::white);
    p.fillRect(0, pixmapCurrentLine*totalLineHeight, width(), totalLineHeight, Qt::black);
    p.drawText(0, pixmapCurrentLine*totalLineHeight, width(), totalLineHeight, Qt::AlignLeft | Qt::AlignBottom, text);
    ++pixmapCurrentLine;
    pixmapCurrentLine = pixmapCurrentLine % allowedEntries;
    p.end();

    QTimer::singleShot(0, this, SLOT(update()));
}

bool NonScrollTextView::event(QEvent *e)
{
    if(e->type() == QEvent::Show && !e->spontaneous() )
    {
        contents.clear();
        updateParameters();     
    }
    return QWidget::event(e);
}

void NonScrollTextView::updateParameters()
{
    QFontMetrics fm = fontMetrics();
    totalLineHeight = fm.boundingRect("M").height();
    allowedEntries = qCeil((float)(height()- 2*MARGIN)/totalLineHeight);
    int toRemove = contents.count() - allowedEntries;
    for(int i=0; i < toRemove ; ++i)
    {
        contents.removeLast();
    }
    textPixmap = QPixmap(width() - 2*MARGIN, totalLineHeight * allowedEntries);
    repaintAll();
}

void NonScrollTextView::repaintAll()
{
    QPainter p(&textPixmap);
    p.fillRect(rect(), Qt::black);
    p.setPen(Qt::white);
    for(int i=0; i < contents.count(); ++i)
    {
        p.drawText(0, height() - (i+1)*totalLineHeight- 2*MARGIN, width(), totalLineHeight, Qt::AlignLeft | Qt::AlignBottom, contents.value(i));
    }
    p.end();
}
