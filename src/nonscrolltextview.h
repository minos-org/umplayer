#ifndef NONSCROLLTEXTVIEW_H
#define NONSCROLLTEXTVIEW_H

#include <QWidget>
#include <QStringList>

class NonScrollTextView : public QWidget
{
    Q_OBJECT
public:
    explicit NonScrollTextView(QWidget *parent = 0);
    ~NonScrollTextView();
    void appendLine(QString text);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);    
    bool event(QEvent *e);

private:
    QStringList contents;
    int allowedEntries;
    int totalLineHeight;
    QPixmap textPixmap;

    int pixmapCurrentLine;

    void updateParameters();
    void repaintAll();



signals:

public slots:



};

#endif // NONSCROLLTEXTVIEW_H
