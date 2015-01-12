/*  umplayer, GUI front-end for mplayer.
    Copyright (C) 2010 Ori Rejwan

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "searchbox.h"
#include "images.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>

SearchBox::SearchBox(QWidget *parent) :
    QWidget(parent)
{
    lineEdit = new QLineEdit(this);
    lineEdit->setFrame(false);
    searchPix = Images::icon("search-icon");
    pressed = false;
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(startSearch()));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

}


void SearchBox::paintEvent(QPaintEvent *pe)
{
    int pixWidth = searchPix.width();
    int pixHeight = searchPix.height();
    QPainter p(this);
    QStyleOptionFrame styleOption;
    styleOption.initFrom(this);
    styleOption.state = QStyle::State_Sunken;
    styleOption.lineWidth = 1;
    styleOption.rect = QRect (0, 0, width(), height());
    style()->drawPrimitive(QStyle::PE_PanelLineEdit, &styleOption, &p, this);
    p.fillRect(searchIconRect, Qt::white);
    p.drawPixmap(width() - pixWidth - 1, (height() - pixHeight)/2, searchPix );
    p.end();
}

void SearchBox::resizeEvent(QResizeEvent *re)
{
    int pixWidth = searchPix.width();
    lineEdit->setGeometry(1, 1, width() - pixWidth - 2, height() - 2);
    searchIconRect = QRect(width() - pixWidth - 1, 1, pixWidth, height() - 2);
}

void SearchBox::mousePressEvent(QMouseEvent *m)
{
    if(searchIconRect.contains(m->pos()))
    {
        pressed = true;
    }
}

void SearchBox::mouseReleaseEvent(QMouseEvent *m)
{
    if(pressed && searchIconRect.contains(m->pos()) && !lineEdit->text().isEmpty())
    {
        emit search(lineEdit->text());
    }
}

void SearchBox::startSearch()
{
    if(!lineEdit->text().isEmpty())
        emit search(lineEdit->text());
}

QSize SearchBox::sizeHint() const
{    
    return QSize(150, lineEdit->sizeHint().height());
}
