#ifndef MEDIABARPANEL_H
#define MEDIABARPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QList>
#include "core.h"


namespace Ui {
    class MediaBarPanel;
}

class PlayControl;
class MediaPanel;
class VolumeControlPanel;
class MyAction;
class MediaBarPanel : public QWidget {
    Q_OBJECT
public:
    MediaBarPanel(QWidget *parent = 0);
    ~MediaBarPanel();
    void setPlayControlActionCollection(QList<QAction*> actions);
    void setMediaPanelActionCollection(QList<QAction*> actions);
    void setVolumeControlActionCollection(QList<QAction*> actions);
    void setCore(Core* c);
    void setRecordAvailable(bool av);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MediaBarPanel *ui;
    PlayControl* playControlPanel;
    MediaPanel* mediaPanel;
    VolumeControlPanel* volumeControlPanel;
    Core* core;


    // Play Control
public slots:
    void setMplayerState(Core::State state);
    void setDuration();
    void gotCurrentTime(double time);
    void updateMediaInfo();
    void displayMessage(QString status);
    void displayPermanentMessage(QString status);
    void setBuffering();



};

#endif // MEDIABARPANEL_H
