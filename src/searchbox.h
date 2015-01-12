#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QWidget>
#include <QLineEdit>
#include <QPixmap>


class SearchBox : public QWidget
{
    Q_OBJECT
public:
    explicit SearchBox(QWidget *parent = 0);
    QSize sizeHint() const;
    void updateText(QString text) { lineEdit->setText(text); }

protected:
    void paintEvent(QPaintEvent *pe);
    void resizeEvent(QResizeEvent *re);
    void mousePressEvent(QMouseEvent *m);
    void mouseReleaseEvent(QMouseEvent *m);

signals:
    void search(QString);

public slots:

    void startSearch();

private:
    QLineEdit* lineEdit;
    QPixmap searchPix;
    QRect searchIconRect;
    bool pressed;

};

#endif // SEARCHBOX_H
