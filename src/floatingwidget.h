/*  umplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2009 Ricardo Villalba <rvm@escomposlinux.org>
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

#ifndef _FLOATING_WIDGET_H_
#define _FLOATING_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include "panelseeker.h"
#include "mybutton.h"
#include "core.h"
#include "widgetactions.h"



class FloatingWidget : public QWidget
{
        Q_OBJECT


public:
	enum Place { Top = 0, Bottom = 1 };
	enum Movement { Upward = 0, Downward = 1 };

	FloatingWidget(QWidget * parent = 0);
	~FloatingWidget();

	//! Show the floating widget over the specified widget.
	void showOver(QWidget * widget, int size = 100, Place place = Bottom);

	void showAnimated(QPoint final_position, Movement movement);



        bool isAnimated() { return _animated; }
        bool autoHide() { return auto_hide; }
        int margin() { return _margin; }
        void setCore(Core* core);
        void setActions(QList<QAction*> actions);
        bool eventFilter(QObject *watched, QEvent *event);

public slots:
        void setMplayerState(Core::State state);
        void setAnimated(bool b) { _animated = b; }
	void setAutoHide(bool b);
        void setMargin(int margin) { _margin = margin; }
        void setVolumeMin();
        void setVolumeMax();
        void setDuration();
        void gotCurrentTime(double t);
#ifndef Q_OS_WIN
	void setBypassWindowManager(bool b);
#endif

protected:
        void paintEvent(QPaintEvent *event);

private slots:
	void animate();
	void checkUnderMouse();

private:
	// Animation variables
	bool _animated;
	QTimer * animation_timer;
	int final_y;
	int current_y;
	Movement current_movement;

	bool auto_hide;
	QTimer auto_hide_timer;

	int _margin;

        QPixmap leftPix;
        QPixmap centerPix;
        QPixmap rightPix;
        PanelSeeker* progressSeeker;
        PanelSeeker* volumeSeeker;
        MyButton* prevButton;
        MyButton* backwardButton;
        MyButton* playPauseButton;
        MyButton* forwardButton;
        MyButton* nextButton;
        MyButton* volMaxButton;
        MyButton* volMinButton;
        MyButton* restoreButton;
        QLabel* elapsedLabel;
        QLabel* totalLabel;
        void setButtonIcon(MyButton* button, QPixmap pix);
        void setPlayPauseIcon(MyButton* button, QPixmap pix);
        Core* core;
        TimeSliderAction* timeSlider;
        VolumeSliderAction* volumeSliderAction;

};

#endif
