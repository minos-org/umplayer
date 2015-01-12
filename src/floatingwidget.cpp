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

#include "floatingwidget.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPainter>
#include <QApplication>
#include <QFont>
#include <QHelpEvent>
#include <QToolTip>
#include <QDebug>
#include "myaction.h"
#include "config.h"

FloatingWidget::FloatingWidget( QWidget * parent )
    : QWidget( parent, Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint )
{
        setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum );
        setFixedHeight(85);
        setMinimumWidth(370);
        setAttribute(Qt::WA_TranslucentBackground, true);

        leftPix = QPixmap(":/Control/bg-float-control-left.png");
        centerPix = QPixmap(":/Control/bg-float-control.png");
        rightPix = QPixmap(":/Control/bg-float-control-right.png");

        elapsedLabel = new QLabel;
        elapsedLabel->setText("00:00:00");
        elapsedLabel->setMargin(0);
        QPalette p = elapsedLabel->palette();
        p.setColor(elapsedLabel->foregroundRole(), Qt::white);
        QFont font = elapsedLabel->font();
        font.setPixelSize(12);
        elapsedLabel->setPalette(p);
        elapsedLabel->setIndent(4);
        elapsedLabel->setFont(font);
        elapsedLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        totalLabel = new QLabel;
        totalLabel->setAlignment(Qt::AlignVCenter | Qt::AlignTrailing);
        totalLabel->setText("00:00:00");
        totalLabel->setPalette(p);
        totalLabel->setFont(font);
        totalLabel->setMargin(0);
        totalLabel->setAlignment(Qt::AlignRight);
        totalLabel->setIndent(4);

        totalLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        progressSeeker = new PanelSeeker;
        progressSeeker->setLeftIcon(QPixmap(":/Control/bg-seeker-left.png"));
        progressSeeker->setCenterIcon(QPixmap(":/Control/bg-seeker.png"));
        progressSeeker->setRightIcon(QPixmap(":/Control/bg-seeker-right.png"));
        progressSeeker->setSingleKnobIcon(QPixmap(":/Control/button-seeker.png"));
        progressSeeker->setProgressIcon(QPixmap(":/Control/bg-seeker-progress.png"));
#ifdef SEEKBAR_RESOLUTION
        progressSeeker->setRange(0, SEEKBAR_RESOLUTION);
#endif
        progressSeeker->setTracking(false);
        progressSeeker->installEventFilter(this);

        QGridLayout* upperLayout = new QGridLayout;
        upperLayout->addWidget(elapsedLabel, 0, 0);
        upperLayout->addWidget(totalLabel, 0, 1);
        upperLayout->addWidget(progressSeeker, 1, 0, 1, 2);
        upperLayout->setContentsMargins(0,0,0,0);
        upperLayout->setVerticalSpacing(0);
        upperLayout->setHorizontalSpacing(0);

        volMinButton = new MyButton;
        setButtonIcon(volMinButton, QPixmap(":/Control/button-mute.png"));
        volMaxButton = new MyButton;
        setButtonIcon(volMaxButton, QPixmap(":/Control/button-max-volume.png"));
        connect(volMinButton, SIGNAL(clicked()), this, SLOT(setVolumeMin()));
        connect(volMaxButton, SIGNAL(clicked()), this, SLOT(setVolumeMax()));

        volumeSeeker = new PanelSeeker;
        volumeSeeker->setLeftIcon(QPixmap(":/Control/bg-volume-left.png"));
        volumeSeeker->setCenterIcon(QPixmap(":/Control/bg-volume.png"));
        volumeSeeker->setRightIcon(QPixmap(":/Control/bg-volume-right.png"));
        volumeSeeker->setProgressIcon(QPixmap(":/Control/bg-volume-progress.png"));
        volumeSeeker->setSingleKnobIcon(QPixmap(":/Control/button-volume.png"));
        volumeSeeker->setLeftRightMargin(5);
        volumeSeeker->setRange(0,100);
        prevButton = new MyButton;
        setButtonIcon(prevButton, QPixmap(":/Control/button-previous.png"));
        backwardButton = new MyButton;
        setButtonIcon(backwardButton, QPixmap(":/Control/button-rewind.png"));
        playPauseButton = new MyButton;
        setPlayPauseIcon(playPauseButton, QPixmap(":/Control/button-play-pause.png"));
        forwardButton = new MyButton;
        setButtonIcon(forwardButton, QPixmap(":/Control/button-fast-forward.png"));
        nextButton = new MyButton;
        setButtonIcon(nextButton, QPixmap(":/Control/button-next.png"));
        restoreButton = new MyButton;
        setButtonIcon(restoreButton, QPixmap(":/Control/button-restore.png"));


        QHBoxLayout* lowerFirstLayout = new QHBoxLayout;
        lowerFirstLayout->addWidget(volMinButton);
        volumeSeeker->setFixedWidth(100);

        lowerFirstLayout->addWidget(volumeSeeker);
        lowerFirstLayout->addWidget(volMaxButton);
        QSpacerItem* spacer1 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
        lowerFirstLayout->addSpacerItem(spacer1);
        lowerFirstLayout->setContentsMargins(0,0,0,0);
        lowerFirstLayout->setSpacing(0);

        QHBoxLayout* lowerSecondLayout = new QHBoxLayout;
        lowerSecondLayout->addWidget(prevButton);
        lowerSecondLayout->addWidget(backwardButton);
        lowerSecondLayout->addWidget(playPauseButton);
        lowerSecondLayout->addWidget(forwardButton);
        lowerSecondLayout->addWidget(nextButton);
        lowerSecondLayout->setContentsMargins(0,0,0,0);
        lowerSecondLayout->setSpacing(10);

        QHBoxLayout* lowerThirdLayout = new QHBoxLayout;
        QSpacerItem* spacer2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
        lowerThirdLayout->addSpacerItem(spacer2);
        lowerThirdLayout->addWidget(restoreButton);
        lowerThirdLayout->setContentsMargins(0,0,0,0);
        lowerThirdLayout->setSpacing(0);

        QHBoxLayout* lowerLayout = new QHBoxLayout;
        lowerLayout->addLayout(lowerFirstLayout);
        lowerLayout->addLayout(lowerSecondLayout);
        lowerLayout->addLayout(lowerThirdLayout);
        lowerLayout->setContentsMargins(0,0,0,0);
        lowerLayout->setSpacing(0);

        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->addLayout(upperLayout);
        mainLayout->addLayout(lowerLayout);
        mainLayout->setContentsMargins(17,10,17,7);
        mainLayout->setSpacing(0);
        setLayout(mainLayout);

	_margin = 0;
	_animated = false;
	animation_timer = new QTimer(this);
	animation_timer->setInterval(2);
	connect( animation_timer, SIGNAL(timeout()), this, SLOT(animate()) );

	connect( &auto_hide_timer, SIGNAL(timeout()),
             this, SLOT(checkUnderMouse()) );
        setAutoHide(false);

}

FloatingWidget::~FloatingWidget() {
}

#ifndef Q_OS_WIN
void FloatingWidget::setBypassWindowManager(bool b) {
	if (b) {
		setWindowFlags(windowFlags() | Qt::X11BypassWindowManagerHint);
	}
	else {
		setWindowFlags(windowFlags() & ~Qt::X11BypassWindowManagerHint);
	}
}
#endif

void FloatingWidget::setAutoHide(bool b) {
	auto_hide = b;

	if (b)
		auto_hide_timer.start(5000);
	else
		auto_hide_timer.stop();
}


void FloatingWidget::showOver(QWidget * widget, int size, Place place) {
	qDebug("FloatingWidget::showOver");

	int w = widget->width() * size / 100;
	int h = height();
	resize( w, h );

	//qDebug("widget x: %d, y: %d, h: %d, w: %d", widget->x(), widget->y(), widget->width(), widget->height());

	int x = (widget->width() - width() ) / 2;
	int y;
	if (place == Top)
		y = 0 + _margin;
	else
		y = widget->height() - height() - _margin;

	QPoint p = widget->mapToGlobal(QPoint(x, y));

	//qDebug("FloatingWidget::showOver: x: %d, y: %d, w: %d, h: %d", x, y, w, h);
	//qDebug("FloatingWidget::showOver: global x: %d global y: %d", p.x(), p.y());
	move(p);

	if (isAnimated()) {
		Movement m = Upward;
		if (place == Top) m = Downward;
		showAnimated(p, m);
	} else {
		show();
	}
}

void FloatingWidget::showAnimated(QPoint final_position, Movement movement) {
	current_movement = movement;
	final_y = final_position.y();

	if (movement == Upward) {
		current_y = final_position.y() + height();
	} else {
		current_y = final_position.y() - height();
	}

	move(x(), current_y);
	show();

	animation_timer->start();
}

void FloatingWidget::animate() {
	if (current_y == final_y) {
		animation_timer->stop();
	} else {
		if (current_movement == Upward) current_y--; else current_y++;
		move(x(), current_y);
	}
}

void FloatingWidget::checkUnderMouse() {
	if (auto_hide) {
		//qDebug("FloatingWidget::checkUnderMouse");
		if ((isVisible()) && (!underMouse())) hide();
	}
}

void FloatingWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.drawPixmap(0, 0, leftPix.width(), height(), leftPix);
    p.drawTiledPixmap(QRect(leftPix.width(), 0, width() - leftPix.width() - rightPix.width(), centerPix.height()), centerPix);
    p.drawPixmap(width() - rightPix.width(), 0, rightPix.width(), height(), rightPix);
}

void FloatingWidget::setButtonIcon(MyButton *button, QPixmap pix)
{
    MyIcon icon;
    int w = pix.width();
    int h = pix.height();
    icon.setPixmap(pix.copy(0, 0, w, h/4 ), MyIcon::Normal, MyIcon::Off);
    icon.setPixmap(pix.copy(0, h/4, w, h/4 ), MyIcon::MouseOver, MyIcon::Off);
    icon.setPixmap(pix.copy(0, h/2, w, h/4 ), MyIcon::MouseDown, MyIcon::Off);
    icon.setPixmap(pix.copy(0, 3*h/4, w, h/4 ), MyIcon::Disabled, MyIcon::Off);

    button->setMyIcon(icon);
    button->setFixedSize(icon.size(MyIcon::Normal, MyIcon::Off));
}

void FloatingWidget::setPlayPauseIcon(MyButton *button, QPixmap pix)
{
    MyIcon icon;
    int w = pix.width();
    int h = pix.height();
    icon.setPixmap(pix.copy(0, 0, w/2, h/4 ), MyIcon::Normal, MyIcon::Off);
    icon.setPixmap(pix.copy(0, h/4, w/2, h/4 ), MyIcon::MouseOver, MyIcon::Off);
    icon.setPixmap(pix.copy(0, h/2, w/2, h/4 ), MyIcon::MouseDown, MyIcon::Off);
    icon.setPixmap(pix.copy(0, 3*h/4, w/2, h/4 ), MyIcon::Disabled, MyIcon::Off);

    icon.setPixmap(pix.copy(w/2, 0, w/2, h/4 ), MyIcon::Normal, MyIcon::On);
    icon.setPixmap(pix.copy(w/2, h/4, w/2, h/4 ), MyIcon::MouseOver, MyIcon::On);
    icon.setPixmap(pix.copy(w/2, h/2, w/2, h/4 ), MyIcon::MouseDown, MyIcon::On);
    icon.setPixmap(pix.copy(w/2, 3*h/4, w/2, h/4 ), MyIcon::Disabled, MyIcon::On);

    button->setMyIcon(icon);
    button->setFixedSize(icon.size(MyIcon::Normal, MyIcon::Off));
}

void FloatingWidget::setActions(QList<QAction*> actions)
{
    backwardButton->setAction(static_cast<MyAction*>(actions.at(0)));
    prevButton->setAction(static_cast<MyAction*>(actions.at(1)));
    playPauseButton->setAction(static_cast<MyAction*>(actions.at(2)));
    nextButton->setAction(static_cast<MyAction*>(actions.at(5)));
    forwardButton->setAction(static_cast<MyAction*>(actions.at(6)));

    timeSlider = static_cast<TimeSliderAction*>(actions.at(7));
    connect(timeSlider, SIGNAL(posChangedExternal(int)), progressSeeker, SLOT(setSliderValue(int)));
    connect(progressSeeker, SIGNAL(sliderMoved(int)), timeSlider, SLOT(setDraggingPosExternal(int)));
    connect(progressSeeker, SIGNAL(valueChanged(int)), timeSlider, SLOT(setPosChangedExternal(int)));

    volumeSliderAction = static_cast<VolumeSliderAction*>(actions.at(8));
    connect(volumeSliderAction, SIGNAL(setValueCalled(int)), volumeSeeker, SLOT(setValue(int)));
    connect(volumeSeeker, SIGNAL(valueChanged(int)), volumeSliderAction, SLOT(emitValueChanged(int)));
    volumeSliderAction->installEventFilter(this);
    volumeSeeker->setEnabled(true);
    restoreButton->setAction(static_cast<MyAction*>(actions.at(9)));



}

void FloatingWidget::setCore(Core *_core)
{
    core = _core;
    connect(core, SIGNAL(stateChanged(Core::State)), this, SLOT(setMplayerState(Core::State)));
    connect(core, SIGNAL(mediaStartPlay()), this, SLOT(setDuration()) );
    connect( core, SIGNAL(showTime(double)), this, SLOT(gotCurrentTime(double)) );
}

void FloatingWidget::setDuration()
{
    totalLabel->setText(Helper::formatTime(core->mdat.duration));
}

void FloatingWidget::gotCurrentTime(double t)
{
    elapsedLabel->setText(Helper::formatTime((int)t));
}

void FloatingWidget::setMplayerState(Core::State st)
{
    if(st == Core::Stopped)
    {
        progressSeeker->setEnabled(false);
    }
    else if(st == Core::Paused || st == Core::Playing)
    {
        progressSeeker->setEnabled(true);
    }
}

bool FloatingWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == volumeSliderAction && event->type() == QEvent::EnabledChange)
    {
        volumeSeeker->setEnabled(volumeSliderAction->isEnabled());
    }
    if (watched == progressSeeker && event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        qreal value = progressSeeker->valueForPos(helpEvent->pos().x())* core->mdat.duration/progressSeeker->maximum();
        if (value >=0 && value <= core->mdat.duration)
        {
            QToolTip::showText(helpEvent->globalPos(),Helper::formatTime(value), progressSeeker);
        } else {
            QToolTip::hideText();
        }
    }
    return false;
}

void FloatingWidget::setVolumeMax()
{
    volumeSeeker->setValue(100);
}

void FloatingWidget::setVolumeMin()
{
    volumeSeeker->setValue(0);
}


#include "moc_floatingwidget.cpp"
