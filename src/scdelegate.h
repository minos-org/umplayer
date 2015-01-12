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

#ifndef SCDELEGATE_H
#define SCDELEGATE_H

#include <QStyledItemDelegate>
#include <QPixmap>
#include <QColor>

class SingleSCResult;

class SCDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
    enum ItemDataRole
    {
        HoverRole = Qt::UserRole + 1,
        ClickRole = Qt::UserRole + 2
    };

    explicit SCDelegate(QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QPixmap scIcon;
    QPixmap hoverPixmap;
    void drawGenreLine(QPainter* painter, QRect area, SingleSCResult* ssr, bool white) const;
    inline QColor cr(QColor color, bool white) const { return white ? Qt::white : color; }

signals:

public slots:

};

#endif // SCDELEGATE_H
