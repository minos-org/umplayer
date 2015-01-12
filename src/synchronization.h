#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include <QWidget>

namespace Ui {
    class Synchronization;
}

class Core;

class Synchronization : public QWidget {
    Q_OBJECT
public:
    Synchronization(QWidget *parent = 0);
    ~Synchronization();
    void setCore(Core* core);
    void setAudioDelayFocus();
    void setSubDelayFocus();

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private:
    Ui::Synchronization *ui;
    Core* core;
public slots:
    void apply();
    void reset();
    void setDefaults();
    void enableApply();
    void mediaStarted();

signals:
    void visibilityChanged();
};

#endif // SYNCHRONIZATION_H
