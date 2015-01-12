#include "equalizer.h"
#include "videoequalizer.h"
#include "audioequalizer.h"
#include "synchronization.h"
#include <QVBoxLayout>

Equalizer::Equalizer(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f)
{
    tabWidget = new QTabWidget(this);
    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(tabWidget);
    vbox->setContentsMargins(5,5,5,5);
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
}


void Equalizer::setVideoEqualizer(VideoEqualizer *vi)
{
    videoEq = vi;
    tabWidget->addTab(videoEq, "Video");
}

void Equalizer::setAudioEqualizer(AudioEqualizer *au)
{
    audioEq = au;
    tabWidget->addTab(audioEq, "Audio");
}

void Equalizer::showAudio()
{
    show();
    tabWidget->setCurrentWidget(audioEq);

}

void Equalizer::showVideo()
{
    show();
    tabWidget->setCurrentWidget(videoEq);
}

void Equalizer::setSynchronization(Synchronization *synch)
{
    synchro = synch;
    tabWidget->addTab(synchro, "Synchronization");
}

void Equalizer::showSubDelay()
{
    show();
    tabWidget->setCurrentWidget(synchro);
    synchro->setSubDelayFocus();
}

void Equalizer::showAudioDelay()
{
    show();
    tabWidget->setCurrentWidget(synchro);
    synchro->setAudioDelayFocus();
}
