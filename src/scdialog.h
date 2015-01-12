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

#ifndef SCDIALOG_H
#define SCDIALOG_H

#include <QWidget>
#include <QPixmap>
#include <QListWidget>
#include "scdataapi.h"

class SearchBox;
class QPaintEvent;

class SCTitleWidget: public QWidget
{
    Q_OBJECT
public:
    SCTitleWidget(QWidget* parent = 0);
    virtual QSize sizeHint() const;
    void setText(QString text);
protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap titleBGPix;
    QPixmap scLogo;
    QString m_text;
};

class SCDelegate;
class SCDataAPI;
class OverlayWidget;



class SCDialog : public QWidget
{
Q_OBJECT
public:
    enum Mode
    {
        Button,
        Search
    };


    void setMode(Mode mode);
    bool eventFilter(QObject *, QEvent *);
    void setLoadingOverlay(bool enable);
    void reset();
    static SCDialog* instance();
    ~SCDialog() {}


private:
    static SCDialog* m_instance;
    QPixmap scLogo;
    //SCTitleWidget* titleWidget;
    QListWidget* streamList;
    SCDelegate* delegate;
    SCDataAPI* api;
    QList<SingleSCResult*> results;
    OverlayWidget* overlay;
    bool overlayVisible;
    QString searchTerm;
    SearchBox* searchBox;

    explicit SCDialog(QWidget *parent = 0);




protected:
    void resizeEvent(QResizeEvent *r);

signals:
    void gotUrls(QString, QString);

public slots:
    void gotAPIReply(SCReply& formattedReply);
    void videoClicked(QListWidgetItem *item);
    void videoDblClicked(QListWidgetItem *item);
    void setSearchTerm(QString term);
    void showContextMenu(QPoint point);


};

#endif // SCDIALOG_H
