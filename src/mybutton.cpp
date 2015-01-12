#include "mybutton.h"
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include "myaction.h"

MyButton::MyButton(QWidget *parent) :
    QAbstractButton(parent), mouseHover(false), state(false), action(0)
{

}


void MyButton::paintEvent(QPaintEvent *e)
{
    QPixmap pix;
    if(isEnabled() && ( isDown() || isChecked()))
    {
        pix = icon.pixmap(MyIcon::MouseDown, state ? MyIcon::On : MyIcon::Off);
    }
    else if(isEnabled() && mouseHover)
    {
        pix = icon.pixmap(MyIcon::MouseOver, state ? MyIcon::On : MyIcon::Off);
    }
    else if(isEnabled())
    {
        pix = icon.pixmap(MyIcon::Normal, state ? MyIcon::On : MyIcon::Off);
    }
    else
    {
        pix = icon.pixmap(MyIcon::Disabled, state ? MyIcon::On : MyIcon::Off);
    }
    QPainter p(this);    
    if(!pix.isNull())
        p.drawPixmap(0,0,pix);    
}


void MyButton::enterEvent(QEvent *)
{
    mouseHover = true;
    update();
}

void MyButton::leaveEvent(QEvent *)
{
    mouseHover = false;
    update();
}

void MyButton::setAction(MyAction *pAction)
{
    action = pAction;
    if(action)
    {
        setEnabled(action->isEnabled());
        action->installEventFilter(this);
        connect(this, SIGNAL(clicked()), action, SLOT(trigger()));
        if( action->isCheckable())
        {
            toggleImage();
            connect(action, SIGNAL(toggled(bool)), this, SLOT(toggleImage()));
        }
    }
}

bool MyButton::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == action)
    {
        if(event->type() == QEvent::ActionChanged)
        {
            setEnabled(action->isEnabled());
        }
    }
    return false;
}

void MyButton::toggleImage()
{    
    if(isCheckable()) setChecked(action->isChecked());
    else setState(action->isChecked());
    update();
}
