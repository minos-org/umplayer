#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <QWidget>
#include <QTabWidget>

class AudioEqualizer;
class VideoEqualizer;
class Synchronization;

class Equalizer : public QWidget
{
Q_OBJECT
public:
    explicit Equalizer(QWidget *parent = 0, Qt::WindowFlags f = 0);
    void setAudioEqualizer(AudioEqualizer* au);
    void setVideoEqualizer(VideoEqualizer* vi);
    void setSynchronization(Synchronization* synch);
    void showAudio();
    void showVideo();    
    void showSubDelay();
    void showAudioDelay();


signals:

public slots:

private:
    QTabWidget* tabWidget;
    AudioEqualizer* audioEq;
    VideoEqualizer* videoEq;
    Synchronization* synchro;


};

#endif // EQUALIZER_H
