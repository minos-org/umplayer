#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QAbstractButton>
#include "myicon.h"

class MyAction;
class MyButton : public QAbstractButton
{
Q_OBJECT
public:
    explicit MyButton(QWidget *parent = 0);
    void setState(bool on) {state = on; }
    void setMyIcon(MyIcon p_icon) { icon = p_icon; }
    MyIcon myIcon() { return icon;}
    void setAction(MyAction* pAction);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    MyIcon icon;
    bool mouseHover;
    bool state;
    MyAction* action;

protected:
    virtual void paintEvent(QPaintEvent *e);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);


signals:

public slots:
    void toggleImage();

};

#endif // MYBUTTON_H
