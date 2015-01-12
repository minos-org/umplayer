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

#ifndef YOUTUBESEARCHBOX_H
#define YOUTUBESEARCHBOX_H

#include <QWidget>
#include <QLineEdit>
#include <QMenu>

class YoutubeSearchBox : public QWidget
{
Q_OBJECT
Q_PROPERTY(QPixmap bgLeft READ leftImage WRITE setLeftImage )
Q_PROPERTY(QPixmap bgRight READ rightImage WRITE setRightImage )
Q_PROPERTY(QPixmap bgCenter READ centerImage WRITE setCenterImage )
Q_PROPERTY(QPixmap arrow READ arrowImage WRITE setArrowImage )
Q_PROPERTY(QPixmap search READ searchImage WRITE setSearchImage)
Q_PROPERTY(QPixmap youtube READ youtubeImage WRITE setYoutubeImage  )
Q_PROPERTY(QPixmap shoutcast READ shoutcastImage WRITE setShoutcastImage)

public:
    enum SearchEngine
    {
        Youtube,
        Shoutcast
    };
    explicit YoutubeSearchBox(QWidget *parent = 0);
    ~YoutubeSearchBox();
    QPixmap leftImage() { return leftPix; }
    QPixmap rightImage() { return rightPix; }
    QPixmap centerImage() { return centerPix; }
    QPixmap arrowImage() { return arrowPix; }
    QPixmap searchImage() {  return searchPix; }
    QPixmap youtubeImage() { return youtubePix; }
    QPixmap shoutcastImage() { return shoutcastPix; }

    void setLeftImage(QPixmap pix) { leftPix = pix; }
    void setCenterImage(QPixmap pix) { centerPix = pix; }
    void setRightImage(QPixmap pix) { rightPix = pix; }
    void setArrowImage(QPixmap pix) { arrowPix = pix; }
    void setSearchImage(QPixmap pix) { searchPix = pix;}
    void setYoutubeImage(QPixmap pix) { youtubePix = pix; setSearchEngine(Youtube); youTubeAction->setIcon(pix); }
    void setShoutcastImage(QPixmap pix) { shoutcastPix = pix; shoutcastAction->setIcon(pix);}

    void setSearchEngine( SearchEngine e );
    bool eventFilter(QObject *watched, QEvent *e);

private:
    QPixmap leftPix;
    QPixmap rightPix;
    QPixmap centerPix;
    QPixmap arrowPix;
    QPixmap searchPix;
    QPixmap youtubePix;
    QPixmap shoutcastPix;
    QPixmap currentSearchEnginePix;
    QLineEdit* lineEdit;
    QMenu* searchList;
    QRect searchEngineArea;
    QRect finderArea;
    SearchEngine engine;
    void setAreas();
    void showSearchList();
    bool empty;

    //search engines
    QAction* youTubeAction;
    QAction* shoutcastAction;

protected:
    void paintEvent(QPaintEvent * e);
    void resizeEvent(QResizeEvent *s);
    void mousePressEvent(QMouseEvent *m);
    QSize sizeHint() const;

signals:
    void search(QString searchTerm, int engine);

public slots:
    void startSearch();
    void changeSearchEngine(QAction* action);
    void setEmpty(QString text);


};

#endif // YOUTUBESEARCHBOX_H
