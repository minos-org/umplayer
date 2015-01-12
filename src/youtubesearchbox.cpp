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


#include "youtubesearchbox.h"
#include <QPainter>
#include <QMouseEvent>
#include <QListWidgetItem>

YoutubeSearchBox::YoutubeSearchBox(QWidget *parent) :
        QWidget(parent), empty(true)
{    
    lineEdit = new QLineEdit(this);
    lineEdit->setObjectName("input");
    lineEdit->setText("Search");
    lineEdit->setFrame(false);        
    lineEdit->setStyleSheet("background-color:transparent; color:gray;");
    lineEdit->installEventFilter(this);
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(startSearch()));
    connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(setEmpty(QString)));
    searchList = new QMenu;
    youTubeAction = new QAction("YouTube", this);
    youTubeAction->setData(Youtube);    
    shoutcastAction = new QAction("Shoutcast", this);
    shoutcastAction->setData(Shoutcast);
    searchList->addAction(youTubeAction);
    searchList->addAction(shoutcastAction);
    connect(searchList, SIGNAL(triggered(QAction*)), this, SLOT(changeSearchEngine(QAction*)));
}

YoutubeSearchBox::~YoutubeSearchBox()
{
    searchList->deleteLater();
}

void YoutubeSearchBox::paintEvent(QPaintEvent *e)
{
    QPainter p(this);    
    p.fillRect(0, (height() - centerPix.height())/2, width(), centerPix.height(), Qt::transparent);
    p.drawPixmap(0, (height() - leftPix.height())/2, leftPix.width(), leftPix.height(), leftPix);    
    p.drawTiledPixmap(leftPix.width(), (height() - centerPix.height())/2, width() - rightPix.width()- leftPix.width(), centerPix.height(), centerPix);    
    p.drawPixmap(width() - rightPix.width(), (height() - rightPix.height())/2, rightPix.width(), rightPix.height(), rightPix);
    p.drawPixmap(leftPix.width(), (height() - currentSearchEnginePix.height())/2, currentSearchEnginePix.width(), currentSearchEnginePix.height(), currentSearchEnginePix);
    p.drawPixmap(leftPix.width() + currentSearchEnginePix.width(), (height() - arrowPix.height())/2, arrowPix.width(), arrowPix.height(), arrowPix);
    p.drawPixmap(finderArea, searchPix);
}

void YoutubeSearchBox::setSearchEngine(SearchEngine e)
{
    engine = e;
    if(e == Youtube) currentSearchEnginePix = youtubePix;
    else if(e == Shoutcast) currentSearchEnginePix = shoutcastPix;
    setAreas();
    update();
}


void YoutubeSearchBox::resizeEvent(QResizeEvent *s)
{
    setAreas();
    lineEdit->setGeometry(QRect(leftPix.width() + searchEngineArea.width(), (height() - currentSearchEnginePix.height())/2, width() - leftPix.width() - rightPix.width() - searchEngineArea.width() - finderArea.width(), currentSearchEnginePix.height()));
}

void YoutubeSearchBox::setAreas()
{
    searchEngineArea = QRect( leftPix.width(), (height() - currentSearchEnginePix.height())/2, currentSearchEnginePix.width() + arrowPix.width(), currentSearchEnginePix.height());
    finderArea = QRect( width() - rightPix.width() - searchPix.width(), (height() - searchPix.height())/2, searchPix.width(), searchPix.height() );
}

void YoutubeSearchBox::mousePressEvent(QMouseEvent *m)
{
    m->accept();
    if(finderArea.contains(m->pos()))
    {
        startSearch();
    }
    if(searchEngineArea.contains(m->pos()))
    {
        showSearchList();
    }
}

void YoutubeSearchBox::startSearch()
{
    if(!empty)
    {
        emit search(lineEdit->text().simplified(), engine );
    }
}

void YoutubeSearchBox::changeSearchEngine(QAction *action)
{
    setSearchEngine(static_cast<SearchEngine>(action->data().toInt()));
}

void YoutubeSearchBox::showSearchList()
{
    QPoint startPoint = QPoint( searchEngineArea.left(), searchEngineArea.bottom());
    searchList->popup(mapToGlobal(startPoint));
}

QSize YoutubeSearchBox::sizeHint() const
{    
    return QSize(220, leftPix.height());
}
bool YoutubeSearchBox::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == lineEdit)
    {
        if(e->type() == QEvent::FocusIn)
        {
            if(empty)
            {
                lineEdit->setText("");
                lineEdit->setStyleSheet("* {background-color:transparent;} :focus { border-style:none; }");
            }
        }
        else if(e->type() == QEvent::FocusOut)
        {
            if(lineEdit->text().isEmpty())
            {
                empty = true;
                lineEdit->setText("Search");
                lineEdit->setStyleSheet("background-color:transparent; color:gray;");
            }
            else empty = false;
        }
    }
    return false;
}

void YoutubeSearchBox::setEmpty(QString text)
{
    if(text.isEmpty()) empty = true;
    else empty = false;
}
