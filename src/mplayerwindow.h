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


#ifndef MPLAYERWINDOW_H
#define MPLAYERWINDOW_H

#include <QWidget>
#include <QSize>
#include <QPoint>

#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QLabel>
#include <QTime>

#ifdef Q_OS_MACX
#include <QGLWidget>
#endif

#include "config.h"
#include "nonscrolltextview.h"

class QWidget;
class QKeyEvent;
class QTimer;


#define ZOOM_STEP 0.05
#define ZOOM_MIN 0.5

#define DELAYED_RESIZE 0


//! Screen is a widget that hides the mouse cursor after some seconds if not moved.

class Screen
{	

public:
        Screen(QWidget* parent);
	~Screen();
        void startCheckMouseTimer();
        void stopCheckMouseTimer();



protected:
        virtual void paintEvent ( QPaintEvent * e );
        QTimer * check_mouse_timer;
        QPoint cursor_pos, last_cursor_pos;
        QTime timeCheck;
        bool floatHide;
        QWidget* owner;
        //! Should be called when a file has started.
        virtual void playingStarted();

        //! Should be called when a file has stopped.
        virtual void playingStopped();




};

//! MplayerLayer can be instructed to not delete the background.

class MplayerLayer :
#ifdef Q_OS_MACX
        public QGLWidget,
#else
        public QWidget,
#endif
        public Screen
{
	Q_OBJECT

public:
        MplayerLayer(QWidget* parent = 0);
	~MplayerLayer();

#if REPAINT_BACKGROUND_OPTION
	//! If b is true, the background of the widget will be repainted as usual.
	/*! Otherwise the background will not repainted when a video is playing. */
	void setRepaintBackground(bool b);

	//! Return true if repainting the background is allowed.
        bool repaintBackground() { return repaint_background; }
#ifdef Q_OS_MACX
        void updateView();
        void setSharedMemory(QString memoryName);
        void stopOpengl();
#endif
#endif

signals:
        void mouseHidden();
        void mouseShown();
        void floatShown();
        void floatHidden();

public slots:
	//! Should be called when a file has started. 
    /*! It's needed to know if the background has to be cleared or not. */
	virtual void playingStarted();
	//! Should be called when a file has stopped.
	virtual void playingStopped();
        virtual void checkMousePos();
#ifdef Q_OS_MACX
        void cleararea_slot();
#endif

#if REPAINT_BACKGROUND_OPTION
protected:
        virtual void mouseMoveEvent( QMouseEvent * e );
        virtual void leaveEvent(QEvent *e);
	virtual void paintEvent ( QPaintEvent * e );
#ifdef Q_OS_MACX
        virtual void resizeEvent(QResizeEvent *r);
#endif

#endif

private:
#if REPAINT_BACKGROUND_OPTION
	bool repaint_background;
#endif
	bool playing;
};

class PictureViewer : public QLabel
{
    Q_OBJECT
public:
    PictureViewer(QWidget* parent = 0);    

protected:
    void paintEvent(QPaintEvent *e);

private:
    QPixmap background;
    QPixmap glow;
    QPixmap logo;

};


class QTimeLine;
class AnimationViewer: public QWidget
{
    Q_OBJECT
public :
        AnimationViewer(QWidget* parent = 0, Qt::WindowFlags f = 0);
        ~AnimationViewer() {}
        QSize sizeHint() const;
        void setText(QString text);
protected:
        void paintEvent(QPaintEvent *);
        void showEvent(QShowEvent *s);
        void hideEvent(QHideEvent *h);

private:
        QTimeLine* timeLine;
        QPixmap stages[12];
        QString m_text;
        bool textVisible;
        QTimer* textTimer;
private slots:
        void frameHasChanged(int);
        void makeTextVisible();

};

class MplayerWindow : public QWidget, public Screen
{
    Q_OBJECT

public:
        MplayerWindow( QWidget* parent = 0, Qt::WindowFlags f = 0);
        ~MplayerWindow();
    
	void showLogo( bool b);
        void showAnimation( bool b, QString text = QString());
        void showFontScanning(bool b);

        MplayerLayer * videoLayer() { return mplayerlayer; }

	void setResolution( int w, int h);
	void setAspect( double asp);
	void setMonitorAspect(double asp);
	void updateVideoWindow();

#if USE_COLORKEY
	void setColorKey(QColor c);
#endif

	void setOffsetX( int );
	int offsetX();

	void setOffsetY( int );
	int offsetY();

	void setZoom( double );
	double zoom();

        void allowVideoMovement(bool b) { allow_video_movement = b; }
        bool isVideoMovementAllowed() { return allow_video_movement; }

	virtual QSize sizeHint () const;
	virtual QSize minimumSizeHint() const;

	virtual bool eventFilter( QObject * watched, QEvent * event );

public slots:
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void incZoom();
	void decZoom();
        virtual void checkMousePos();
        /*! It's needed to know if the background has to be cleared or not. */
        virtual void playingStarted();
        //! Should be called when a file has stopped.
        virtual void playingStopped();
        void appendScanningMessage(QString message);
        void testForScanningFinished(QString message);


#if DELAYED_RESIZE
protected slots:
	void resizeLater();
#endif

protected:
	virtual void retranslateStrings();
	virtual void changeEvent ( QEvent * event ) ;

    virtual void resizeEvent( QResizeEvent * e);
    virtual void mouseReleaseEvent( QMouseEvent * e);
	virtual void mouseDoubleClickEvent( QMouseEvent * e );
	virtual void wheelEvent( QWheelEvent * e );
	void moveLayer( int offset_x, int offset_y );
        virtual void mouseMoveEvent( QMouseEvent * e );
        virtual void leaveEvent(QEvent *e);
        virtual void paintEvent(QPaintEvent *e);



signals:
    //void rightButtonReleased( QPoint p );
	void doubleClicked();
	void leftClicked();
	void rightClicked();
	void middleClicked();
	void xbutton1Clicked(); // first X button
	void xbutton2Clicked(); // second X button
	void keyPressed(QKeyEvent * e);
	void wheelUp();
	void wheelDown();
	void mouseMoved(QPoint);
        void mouseMovedDiff(QPoint);
        void mouseHidden();
        void mouseShown();
        void floatShown();
        void floatHidden();
        void fontBeingScanned(QString);


protected:
    int video_width, video_height;
    double aspect;
	double monitoraspect;

	MplayerLayer * mplayerlayer;
        NonScrollTextView* fontScanView;
        PictureViewer * logo;
        AnimationViewer* loadingAnimation;

	// Zoom and moving
	int offset_x, offset_y;
	double zoom_factor;

	// Original pos and dimensions of the mplayerlayer
	// before zooming or moving
	int orig_x, orig_y;
	int orig_width, orig_height;

	bool allow_video_movement;
        QPoint mousePressPos;

#if DELAYED_RESIZE
	QTimer * resize_timer;
#endif
};


#endif

