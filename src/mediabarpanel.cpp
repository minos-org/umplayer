#include "mediabarpanel.h"
#include "ui_mediabarpanel.h"
#include "playcontrol.h"
#include "mediapanel.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include "qpropertysetter.h"
#include "colorutils.h"
#include "qpropertysetter.h"
#include "volumecontrolpanel.h"



MediaBarPanel::MediaBarPanel(QWidget *parent) :
    QWidget(parent),ui(new Ui::MediaBarPanel),core(0)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(53);
    QHBoxLayout* layout = new QHBoxLayout;
    playControlPanel = new PlayControl(this);
    IconSetter::instance()->playControl = playControlPanel;
    volumeControlPanel = new VolumeControlPanel(this);
    volumeControlPanel->setObjectName("volume-control-panel");
    mediaPanel = new MediaPanel(this);
    mediaPanel->setObjectName("media-panel");
    IconSetter::instance()->mediaPanel = mediaPanel;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(playControlPanel);
    layout->addWidget(mediaPanel);
    layout->addWidget(volumeControlPanel);            
    setLayout(layout);

}

MediaBarPanel::~MediaBarPanel()
{
    delete ui;
}

void MediaBarPanel::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MediaBarPanel::setPlayControlActionCollection(QList<QAction *>actions)
{
    playControlPanel->setActionCollection(actions);
}

void MediaBarPanel::setMediaPanelActionCollection(QList<QAction *>actions)
{
    mediaPanel->setActionCollection(actions);
}

void MediaBarPanel::setMplayerState(Core::State state)
{
    mediaPanel->setMplayerState((int)state);    
}
void MediaBarPanel::setCore(Core *c)
{
    core = c;
    connect(core, SIGNAL(mediaStartPlay()), this, SLOT(setDuration()) );
    connect( core, SIGNAL(showTime(double)), this, SLOT(gotCurrentTime(double)) );
    connect( core, SIGNAL(mediaInfoChanged()), this, SLOT(updateMediaInfo()) );    
    connect( core, SIGNAL(buffering()), this, SLOT(setBuffering()) );

}

void MediaBarPanel::setDuration()
{
    mediaPanel->setDuration(core->mdat.duration);
}

void MediaBarPanel::setVolumeControlActionCollection(QList<QAction *>actions)
{
    volumeControlPanel->setActionCollection(actions);
}

void MediaBarPanel::gotCurrentTime(double time)
{
    mediaPanel->setElapsedText(Helper::formatTime((int)time));    
}

void MediaBarPanel::updateMediaInfo()
{
    mediaPanel->setMediaLabelText(core->mdat.displayName());
}

void MediaBarPanel::displayMessage(QString status)
{
    mediaPanel->setStatusText(status);
}

void MediaBarPanel::displayPermanentMessage(QString status)
{
    mediaPanel->setStatusText(status, 0);
}

void MediaBarPanel::setRecordAvailable(bool av)
{
    playControlPanel->setRecordEnabled(av);
}

void MediaBarPanel::setBuffering()
{
    mediaPanel->setBuffering(true);
}
