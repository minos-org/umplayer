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

#include "mplayerwindow.h"
#include "global.h"
#include "desktopinfo.h"
#include "colorutils.h"

#ifndef MINILIB
#include "images.h"
#endif

#include "preferences.h"
#include "global.h"

#include <QLabel>
#include <QTimer>
#include <QCursor>
#include <QEvent>
#include <QLayout>
#include <QPixmap>
#include <QPainter>
#include <QDebug>
#include <QTimeLine>
#include <QApplication>
#include <QDesktopWidget>
#ifdef Q_OS_MACX
    #include "objc++bridge.h"
    #include "globaldataclass.h"
#endif
#include "qpropertysetter.h"

#if DELAYED_RESIZE
#include <QTimer>
#endif

using namespace Global;


Screen::Screen(QWidget* parent)
    :owner(parent)
{
        owner->setMouseTracking(TRUE);
        owner->setFocusPolicy( Qt::NoFocus );
        owner->setMinimumSize( QSize(0,0) );

	cursor_pos = QPoint(0,0);
	last_cursor_pos = QPoint(0,0);
        floatHide = false;

        check_mouse_timer = new QTimer(owner);
        check_mouse_timer->setInterval(1000);

	// Change attributes
        owner->setAttribute(Qt::WA_NoSystemBackground);
	//setAttribute(Qt::WA_StaticContents);
        //setAttribute( Qt::WA_OpaquePaintEvent );
        owner->setAttribute(Qt::WA_PaintOnScreen);
        owner->setAttribute(Qt::WA_PaintUnclipped);
	//setAttribute(Qt::WA_PaintOutsidePaintEvent);

}

Screen::~Screen() {
}

void Screen::startCheckMouseTimer()
{
    check_mouse_timer->start();
    floatHide = false;
}

void Screen::stopCheckMouseTimer()
{
    check_mouse_timer->stop();
    owner->setCursor(QCursor(Qt::ArrowCursor));
}

void Screen::paintEvent( QPaintEvent * e ) {
	//qDebug("Screen::paintEvent");
        QPainter painter(owner);
	painter.eraseRect( e->rect() );
	//painter.fillRect( e->rect(), QColor(255,0,0) );
}




void Screen::playingStarted() {
}

void Screen::playingStopped() {
}

/* ---------------------------------------------------------------------- */

MplayerLayer::MplayerLayer(QWidget* parent):
#ifdef Q_OS_MACX
        QGLWidget(parent),
#else
        QWidget(parent),
#endif
        Screen(this)
{
#if REPAINT_BACKGROUND_OPTION
	repaint_background = true;
#endif
        setAttribute(Qt::WA_PaintOnScreen, true);
	playing = false;
        connect( check_mouse_timer, SIGNAL(timeout()), this, SLOT(checkMousePos()) );
        timeCheck.start();
#ifdef Q_OS_MACX
        globaldata.gl = this;
#endif
}

MplayerLayer::~MplayerLayer() {
}

void MplayerLayer::checkMousePos() {
        //qDebug("Screen::checkMousePos");
        if(!pref->fullscreen) return;

        if ( cursor_pos == last_cursor_pos ) {
                //qDebug(" same pos");
            if (underMouse()) {
                        //qDebug(" hiding mouse cursor");
                if(cursor().shape()!= Qt::BlankCursor )
                    {
                        setCursor(QCursor(Qt::BlankCursor));
                        emit mouseHidden();
                    }
                if(!floatHide)
                {
                    floatHide = true;
                    emit floatHidden();
                }
                if(cursor().shape()== Qt::BlankCursor && floatHide )
                {
                    check_mouse_timer->stop();
                }
                }
        } else {
                last_cursor_pos = cursor_pos;
        }
}

#if REPAINT_BACKGROUND_OPTION
void MplayerLayer::mouseMoveEvent( QMouseEvent * e ) {
        //qDebug("Screen::mouseMoveEvent");
        if(!pref->fullscreen) return;

        cursor_pos = e->pos();
        if (cursor().shape() != Qt::ArrowCursor) {
                //qDebug(" showing mouse cursor" );
                setCursor(QCursor(Qt::ArrowCursor));
                emit mouseShown();
        }
        if(floatHide  && e->globalY() > QApplication::desktop()->height()*2/3)
        {
            emit floatShown();
            floatHide = false;
        }
        if(timeCheck.elapsed() > 100)
        {
            check_mouse_timer->start();
            timeCheck.restart();
        }
}

void MplayerLayer::leaveEvent(QEvent *)
{
    if(!pref->fullscreen) return;
    //floatHide = true;
    setCursor(QCursor(Qt::ArrowCursor));

}

void MplayerLayer::setRepaintBackground(bool b) {
	qDebug("MplayerLayer::setRepaintBackground: %d", b);
	repaint_background = b;
}

void MplayerLayer::paintEvent( QPaintEvent * e ) {
	//qDebug("MplayerLayer::paintEvent: allow_clearing: %d", allow_clearing);
	if (repaint_background || !playing) {
		//qDebug("MplayerLayer::paintEvent: painting");
                Screen::paintEvent(e);
	}
}
#endif

void MplayerLayer::playingStarted() {
	qDebug("MplayerLayer::playingStarted");
	repaint();
	playing = true;

	Screen::playingStarted();
}
#ifdef Q_OS_MACX
void MplayerLayer::cleararea_slot() {
    cleararea_bridge();
}


void MplayerLayer::resizeEvent(QResizeEvent *event) {

    changezoom(event->size().width(), event->size().height());
}

void MplayerLayer::updateView()
{
        updateGL();
}

void MplayerLayer::setSharedMemory(QString memoryName)
{
    makeCurrent();
    startObjcFunction(memoryName.toAscii().data());
    //cleararea_bridge();
    QTimer::singleShot(0, this, SLOT(cleararea_slot()));
}

void MplayerLayer::stopOpengl()
{
    stopObjcFunction();
}

#endif

void MplayerLayer::playingStopped() {
        qDebug("MplayerLayer::playingStopped");
        playing = false;
        repaint();

        Screen::playingStopped();
}



/* ---------------------------------------------------------------------- */

PictureViewer::PictureViewer(QWidget *parent)
    :QLabel(parent)
{
    background = IconSetter::instance()->logoBgIcon();
    glow = IconSetter::instance()->logoSplashIcon();
    logo = IconSetter::instance()->logoIcon();

}

void PictureViewer::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    //p.drawTiledPixmap(rect(), background);
    //p.drawPixmap(rect().center() - glow.rect().center(), glow);
    //QPixmap scaledPix = logo;
    //if(logo.width() > width() || logo.height() > height())
    //{
        //scaledPix = logo.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //}
    //p.drawPixmap(rect().center() - scaledPix.rect().center(), scaledPix);

    p.end();
}




/*-------------------------------------------------------------------------*/

AnimationViewer::AnimationViewer(QWidget *parent, Qt::WindowFlags f)
    :QWidget(parent,f)
{
    timeLine = new QTimeLine(1000, this);
    timeLine->setLoopCount(0);
    timeLine->setFrameRange(1, 13);
    for(int i=0; i < 12; ++i)
    {
        stages[i] = Images::icon(QString("stage%1").arg(i+1, 2, 10, QChar('0')));
    }
    timeLine->setCurveShape(QTimeLine::LinearCurve);
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(frameHasChanged(int)));
    textVisible = false;
    textTimer = new QTimer(this);
    textTimer->setInterval(1000);
    textTimer->setSingleShot(true);
    connect(textTimer, SIGNAL(timeout()), SLOT(makeTextVisible()));
}

void AnimationViewer::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter p(this);
    const QPixmap pix = stages[qMax(1,qMin(12,timeLine->currentFrame()))-1];
    QPixmap scaledPix = pix.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    p.drawPixmap(rect().center() - scaledPix.rect().center(), scaledPix);
    if(!m_text.isEmpty() && textVisible && !pref->fontCacheBuilt)
    {
        QFont font = p.font();
        font.setPixelSize(16);
        font.setBold(true);
        p.setFont(font);
        p.setPen(Qt::white);
        p.drawText(rect(), Qt::AlignCenter| Qt::TextWordWrap, m_text);
    }
    p.end();
}

void AnimationViewer::frameHasChanged(int)
{
    repaint();
}

void AnimationViewer::showEvent(QShowEvent *s)
{
    QWidget::showEvent(s);
    if(!s->spontaneous())
        timeLine->start();
    textTimer->start();
}

void AnimationViewer::hideEvent(QHideEvent *h)
{
    QWidget::hideEvent(h);
    if(!h->spontaneous())
        timeLine->stop();
    textTimer->stop();
    textVisible = false;
}

void AnimationViewer::makeTextVisible()
{
    textVisible = true;
    update();
}

QSize AnimationViewer::sizeHint() const
{
    QSize sz = stages[0].size();
    QSize sz2 = fontMetrics().boundingRect(m_text).size();
    return sz.expandedTo(sz2);
}

void AnimationViewer::setText(QString text)
{
    m_text = text;
}

/*--------------------------------------------------------------------------*/
MplayerWindow::MplayerWindow(QWidget* parent, Qt::WindowFlags f )
        : QWidget(parent, f), Screen(this) , allow_video_movement(false)
{
	offset_x = 0;
	offset_y = 0;
	zoom_factor = 1.0;
        QObject::connect( check_mouse_timer, SIGNAL(timeout()), this, SLOT(checkMousePos()) );
        timeCheck.start();


	setAutoFillBackground(true);
        ColorUtils::setBackgroundColor( this, QColor(0,0,0) );
        mplayerlayer = new MplayerLayer( this );
        mplayerlayer->setAutoFillBackground(TRUE);
        mplayerlayer->hide();

        logo = new PictureViewer( mplayerlayer );
        fontScanView = new NonScrollTextView(mplayerlayer);
        loadingAnimation = new AnimationViewer(mplayerlayer);

        loadingAnimation->setAutoFillBackground(TRUE);
        fontScanView->setAutoFillBackground(true);
#if QT_VERSION >= 0x040400
	logo->setAttribute(Qt::WA_NativeWindow); // Otherwise the logo is not visible in Qt 4.4
        loadingAnimation->setAttribute(Qt::WA_NativeWindow);
        fontScanView->setAttribute(Qt::WA_NativeWindow);
#else
	logo->setAttribute(Qt::WA_PaintOnScreen); // Fixes the problem if compiled with Qt < 4.4
        loadingAnimation->setAttribute(Qt::WA_PaintOnScreen);
        fontScanView->setAttribute(Qt::WA_PaintOnScreen);
#endif

        ColorUtils::setBackgroundColor( logo, QColor(0,0,0) );
        ColorUtils::setBackgroundColor( loadingAnimation, QColor(0,0,0) );
        ColorUtils::setBackgroundColor( fontScanView, QColor(0,0,0) );
        loadingAnimation->hide();
        fontScanView->hide();

        QVBoxLayout * mplayerlayerLayout = new QVBoxLayout( this /* mplayerlayer */);
        mplayerlayerLayout->addWidget( logo);
        mplayerlayerLayout->addWidget( loadingAnimation, 0, Qt::AlignHCenter | Qt::AlignVCenter );
        mplayerlayerLayout->addWidget(fontScanView);
        mplayerlayerLayout->setContentsMargins(0,0,0,0);

    aspect = (double) 4 / 3;
	monitoraspect = 0;

	setSizePolicy( QSizePolicy::Expanding , QSizePolicy::Expanding );
	setFocusPolicy( Qt::StrongFocus );

	installEventFilter(this);
	mplayerlayer->installEventFilter(this);
	//logo->installEventFilter(this);

#if DELAYED_RESIZE
	resize_timer = new QTimer(this);
	resize_timer->setSingleShot(true);
	resize_timer->setInterval(50);
	connect( resize_timer, SIGNAL(timeout()), this, SLOT(resizeLater()) );
#endif

	retranslateStrings();
}

MplayerWindow::~MplayerWindow() {
}

void MplayerWindow::checkMousePos() {
        //qDebug("Screen::checkMousePos");
        if(!pref->fullscreen) return;

        if ( cursor_pos == last_cursor_pos ) {
                //qDebug(" same pos");
            if (underMouse()) {
                        //qDebug(" hiding mouse cursor");
                if(cursor().shape()!= Qt::BlankCursor )
                    {
                        setCursor(QCursor(Qt::BlankCursor));
                        emit mouseHidden();
                    }
                if(!floatHide)
                {
                    floatHide = true;
                    emit floatHidden();
                }
                if(cursor().shape()== Qt::BlankCursor && floatHide )
                {
                    check_mouse_timer->stop();
                }
                }
        } else {
                last_cursor_pos = cursor_pos;
        }
}

void MplayerWindow::mouseMoveEvent( QMouseEvent * e ) {
        //qDebug("Screen::mouseMoveEvent");
        if(!pref->fullscreen) return;

        cursor_pos = e->pos();
        if (cursor().shape() != Qt::ArrowCursor) {
                //qDebug(" showing mouse cursor" );
                setCursor(QCursor(Qt::ArrowCursor));
                emit mouseShown();

        }
        if(floatHide  && e->globalY() > QApplication::desktop()->height()*2/3)
        {
            emit floatShown();
            floatHide = false;
        }
        if(timeCheck.elapsed() > 100)
        {
            check_mouse_timer->start();
            timeCheck.restart();
        }
}

void MplayerWindow::leaveEvent(QEvent *)
{
    if(!pref->fullscreen) return;
    //floatHide = true;
    setCursor(QCursor(Qt::ArrowCursor));
}


#if USE_COLORKEY
void MplayerWindow::setColorKey( QColor c ) {
	ColorUtils::setBackgroundColor( mplayerlayer, c );
}
#endif

void MplayerWindow::retranslateStrings() {
	//qDebug("MplayerWindow::retranslateStrings");

}

void MplayerWindow::showLogo( bool b)
{
    if (b) {
        logo->show();
        loadingAnimation->hide();
        fontScanView->hide();
        mplayerlayer->hide();
    }
    else
    {
        logo->hide();
        if(!loadingAnimation->isVisible())
            mplayerlayer->show();
    }
}
void MplayerWindow::showAnimation(bool b, QString text)
{
        loadingAnimation->setText(text);
        if (b)
        {
            loadingAnimation->show();
            logo->hide();
            fontScanView->hide();
            mplayerlayer->hide();
        }
        else
        {
            loadingAnimation->hide();
            if(!logo->isVisible())
                mplayerlayer->show();

        }
}

void MplayerWindow::showFontScanning(bool b)
{
    if (b)
    {
        fontScanView->show();
        logo->hide();
        loadingAnimation->hide();
        mplayerlayer->hide();

    }
    else
    {
        fontScanView->hide();
        if(!logo->isVisible())
            mplayerlayer->show();
    }

}

/*
void MplayerWindow::changePolicy() {
	setSizePolicy( QSizePolicy::Preferred , QSizePolicy::Preferred  );
}
*/

void MplayerWindow::setResolution( int w, int h)
{
    video_width = w;
    video_height = h;

    //mplayerlayer->move(1,1);
    updateVideoWindow();
}


void MplayerWindow::resizeEvent( QResizeEvent * /* e */)
{
   /*qDebug("MplayerWindow::resizeEvent: %d, %d",
	   e->size().width(), e->size().height() );*/

#if !DELAYED_RESIZE
	offset_x = 0;
	offset_y = 0;

    updateVideoWindow();
	setZoom(zoom_factor);
#else
	resize_timer->start();
#endif
}

#if DELAYED_RESIZE
void MplayerWindow::resizeLater() {
	offset_x = 0;
	offset_y = 0;

    updateVideoWindow();
	setZoom(zoom_factor);
}
#endif

void MplayerWindow::setMonitorAspect(double asp) {
	monitoraspect = asp;
}

void MplayerWindow::setAspect( double asp) {
    aspect = asp;
	if (monitoraspect!=0) {
		aspect = aspect / monitoraspect * DesktopInfo::desktop_aspectRatio(this);
	}
	updateVideoWindow();
}


void MplayerWindow::updateVideoWindow()
{
	//qDebug("MplayerWindow::updateVideoWindow");

    //qDebug("aspect= %f", aspect);

    int w_width = size().width();
    int w_height = size().height();

	int w = w_width;
	int h = w_height;
	int x = 0;
	int y = 0;

	if (aspect != 0) {
	    int pos1_w = w_width;
	    int pos1_h = w_width / aspect + 0.5;

	    int pos2_h = w_height;
	    int pos2_w = w_height * aspect + 0.5;

	    //qDebug("pos1_w: %d, pos1_h: %d", pos1_w, pos1_h);
	    //qDebug("pos2_w: %d, pos2_h: %d", pos2_w, pos2_h);

	    if (pos1_h <= w_height) {
		//qDebug("Pos1!");
			w = pos1_w;
			h = pos1_h;

			y = (w_height - h) /2;
	    } else {
		//qDebug("Pos2!");
			w = pos2_w;
			h = pos2_h;

			x = (w_width - w) /2;
	    }
	}

    mplayerlayer->move(x,y);
    mplayerlayer->resize(w, h);

	orig_x = x;
	orig_y = y;
	orig_width = w;
	orig_height = h;

    //qDebug( "w_width: %d, w_height: %d", w_width, w_height);
    //qDebug("w: %d, h: %d", w,h);
}


void MplayerWindow::mouseReleaseEvent( QMouseEvent * e) {
    qDebug( "MplayerWindow::mouseReleaseEvent" );

	if (e->button() == Qt::LeftButton) {
		e->accept();
		emit leftClicked();
	}
	else
	if (e->button() == Qt::MidButton) {
		e->accept();
		emit middleClicked();
	}
	else
	if (e->button() == Qt::XButton1) {
		e->accept();
		emit xbutton1Clicked();
	}
	else
	if (e->button() == Qt::XButton2) {
		e->accept();
		emit xbutton2Clicked();
	}
	else
    if (e->button() == Qt::RightButton) {
		e->accept();
		//emit rightButtonReleased( e->globalPos() );
		emit rightClicked();
    }
	else {
		e->ignore();
	}
}

void MplayerWindow::mouseDoubleClickEvent( QMouseEvent * e ) {
	if (e->button() == Qt::LeftButton) {
		e->accept();
		emit doubleClicked();
	} else {
		e->ignore();
	}
}

void MplayerWindow::wheelEvent( QWheelEvent * e ) {
    qDebug("MplayerWindow::wheelEvent: delta: %d", e->delta());
    e->accept();

	if (e->orientation() == Qt::Vertical) {
	    if (e->delta() >= 0)
	        emit wheelUp();
	    else
	        emit wheelDown();
	} else {
		qDebug("MplayerWindow::wheelEvent: horizontal event received, doing nothing");
	}
}

bool MplayerWindow::eventFilter( QObject * /*watched*/, QEvent * event ) {
	//qDebug("MplayerWindow::eventFilter");

	if ( (event->type() == QEvent::MouseMove) ||
         (event->type() == QEvent::MouseButtonRelease) )
	{
		QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);

		if (event->type() == QEvent::MouseMove) {
			emit mouseMoved(mouse_event->pos());
                        if( mouse_event->buttons().testFlag(Qt::LeftButton))
                        {
                            emit mouseMovedDiff( mouse_event->globalPos() - mousePressPos);
                            mousePressPos = mouse_event->globalPos();
                            qDebug() << mousePressPos << "here";
                        }
		}
	}
        if(event->type() == QEvent::MouseButtonPress)
        {
                QMouseEvent *mouse_event = static_cast<QMouseEvent *>(event);
                mousePressPos = mouse_event->globalPos();
        }

	return false;
}

QSize MplayerWindow::sizeHint() const {
	//qDebug("MplayerWindow::sizeHint");
	return QSize( video_width, video_height );
}

QSize MplayerWindow::minimumSizeHint () const {
	return QSize(0,0);
}

void MplayerWindow::setOffsetX( int d) {
	offset_x = d;
	mplayerlayer->move( orig_x + offset_x, mplayerlayer->y() );
}

int MplayerWindow::offsetX() { return offset_x; }

void MplayerWindow::setOffsetY( int d) {
	offset_y = d;
	mplayerlayer->move( mplayerlayer->x(), orig_y + offset_y );
}

int MplayerWindow::offsetY() { return offset_y; }

void MplayerWindow::setZoom( double d) {
	zoom_factor = d;
	offset_x = 0;
	offset_y = 0;

	int x = orig_x;
	int y = orig_y;
	int w = orig_width;
	int h = orig_height;

	if (zoom_factor != 1.0) {
		w = w * zoom_factor;
		h = h * zoom_factor;

		// Center
		x = (width() - w) / 2;
		y = (height() -h) / 2;
	}

	mplayerlayer->move(x,y);
	mplayerlayer->resize(w,h);
}

double MplayerWindow::zoom() { return zoom_factor; }

void MplayerWindow::moveLayer( int offset_x, int offset_y ) {
	int x = mplayerlayer->x();
	int y = mplayerlayer->y();

	mplayerlayer->move( x + offset_x, y + offset_y );
}

void MplayerWindow::moveLeft() {
	if ((allow_video_movement) || (mplayerlayer->x()+mplayerlayer->width() > width() ))
		moveLayer( -16, 0 );
}

void MplayerWindow::moveRight() {
	if ((allow_video_movement) || ( mplayerlayer->x() < 0 ))
		moveLayer( +16, 0 );
}

void MplayerWindow::moveUp() {
	if ((allow_video_movement) || (mplayerlayer->y()+mplayerlayer->height() > height() ))
		moveLayer( 0, -16 );
}

void MplayerWindow::moveDown() {
	if ((allow_video_movement) || ( mplayerlayer->y() < 0 ))
		moveLayer( 0, +16 );
}

void MplayerWindow::incZoom() {
	setZoom( zoom_factor + ZOOM_STEP );
}

void MplayerWindow::decZoom() {
	double zoom = zoom_factor - ZOOM_STEP;
	if (zoom < ZOOM_MIN) zoom = ZOOM_MIN;
	setZoom( zoom );
}

// Language change stuff
void MplayerWindow::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		retranslateStrings();
	} else {
		QWidget::changeEvent(e);
	}
}

void MplayerWindow::paintEvent(QPaintEvent *e)
{
    Screen::paintEvent(e);
}

void MplayerWindow::playingStarted() {
}

void MplayerWindow::playingStopped() {
}

void MplayerWindow::appendScanningMessage(QString message)
{
    if(!fontScanView->isVisible())
    {
        showFontScanning(true);
        emit fontBeingScanned(tr("Please wait while UMP builds font cache. This should take less than a few minutes and should only happen once."));
    }
    fontScanView->appendLine(message);
}

void MplayerWindow::testForScanningFinished(QString message)
{
    if(fontScanView->isVisible() && !message.trimmed().startsWith("Scanning"))
    {
        showFontScanning(false);
    }
}


#include "moc_mplayerwindow.cpp"
