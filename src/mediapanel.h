#ifndef MEDIAPANEL_H
#define MEDIAPANEL_H

#include <QtGui/QWidget>
#include <QPixmap>
#include <QLabel>
#include "ui_mediapanel.h"
#include "mybutton.h"
#include "panelseeker.h"

class TimeSliderAction;
class ScrollingLabel : public QWidget
{
    Q_OBJECT

public:
    ScrollingLabel(QWidget* parent=0);
    ~ScrollingLabel(){}
    QString text() { return mText; }
    void setText( QString text);


private:
    QString mText;
    void updateLabel();
    int scrollPos;
    int timerId;
    QRect textRect;
    static const int gap = 10;        


protected:
    void paintEvent(QPaintEvent *);
    void changeEvent(QEvent *);
    void resizeEvent(QResizeEvent *);
    QSize sizeHint() const;
private slots:
    void timerEvent(QTimerEvent *);
};

class MediaPanel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QPixmap bgLeft READ bgLeftPix WRITE setBgLeftPix)
    Q_PROPERTY(QPixmap bgRight READ bgRightPix WRITE setBgRightPix)
    Q_PROPERTY(QPixmap bgCenter READ bgCenterPix WRITE setBgCenterPix)

public:
    MediaPanel(QWidget *parent = 0);
    ~MediaPanel();
    QPixmap bgLeftPix() { return leftBackground ;}
    void setBgLeftPix( QPixmap pix){ leftBackground = pix; }
    QPixmap bgRightPix() { return rightBackground ;}
    void setBgRightPix( QPixmap pix){ rightBackground = pix; }
    QPixmap bgCenterPix() { return centerBackground ;}
    void setBgCenterPix( QPixmap pix){ centerBackground = pix; }
    void setShuffleIcon( MyIcon icon );
    void setRepeatIcon(MyIcon icon);
    void setElapsedText(QString text) {
        elapsedLabel->setText(text);
        if(seeker->states().testFlag(PanelSeeker::Buffering))
            setBuffering(false);
        }
    void setTotalText( QString text) { totalLabel->setText(text); }
    void setActionCollection(QList<QAction*> actions);
    void setMplayerState(int state);
    void setDuration(int duration);
    void setMediaLabelText(QString text);
    void setStatusText(QString text, int time = 2000);
    void setBuffering(bool enable);
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::MediaPanelClass ui;
    QPixmap leftBackground;
    QPixmap centerBackground;
    QPixmap rightBackground;
    ScrollingLabel* mediaLabel;
    PanelSeeker* seeker;
    MyButton* repeatButton;
    MyButton* shuffleButton;
    QLabel* elapsedLabel;
    QLabel* totalLabel;
    TimeSliderAction* timeSlider;
    QString originalTitle;
    QTimer* timer;    
    int duration;
private slots:
    void reverseStatus();

protected:
    void paintEvent(QPaintEvent *);

public:
    friend class IconSetter;
};




#endif // MEDIAPANEL_H
