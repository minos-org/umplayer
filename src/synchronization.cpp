#include "synchronization.h"
#include "ui_synchronization.h"
#include "core.h"
#include <QShowEvent>

Synchronization::Synchronization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Synchronization)
{
    ui->setupUi(this);
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(reset()));
    connect(ui->setDefaultButton, SIGNAL(clicked()), this, SLOT(setDefaults()));
    connect(ui->audioDelaySpin, SIGNAL(valueChanged(int)), this, SLOT(enableApply()));
    connect(ui->subtitlesDelaySpin, SIGNAL(valueChanged(int)), this, SLOT(enableApply()));    
}

Synchronization::~Synchronization()
{
    delete ui;
}

void Synchronization::changeEvent(QEvent *e)
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

void Synchronization::apply()
{
    core->setAudioDelay(ui->audioDelaySpin->value());
    core->setSubDelay(ui->subtitlesDelaySpin->value());
    ui->applyButton->setEnabled(false);
}

void Synchronization::reset()
{    
    ui->audioDelaySpin->setValue(0);
    ui->subtitlesDelaySpin->setValue(0);
    apply();
}

void Synchronization::setDefaults()
{

}

void Synchronization::setCore(Core *_core)
{
    core = _core;
    connect(core, SIGNAL(mediaStartPlay()), this, SLOT(mediaStarted()));
}

void Synchronization::enableApply()
{
    ui->applyButton->setEnabled(true);
}

void Synchronization::showEvent(QShowEvent *e)
{
    if(!e->spontaneous())
    {        
        emit visibilityChanged();
    }
}

void Synchronization::hideEvent(QHideEvent *e)
{
    emit visibilityChanged();
}

void Synchronization::setAudioDelayFocus()
{
    ui->audioDelaySpin->setFocus();
}

void Synchronization::setSubDelayFocus()
{
    ui->subtitlesDelaySpin->setFocus();
}

void Synchronization::mediaStarted()
{
    ui->audioDelaySpin->setValue(core->mset.audio_delay);
    ui->subtitlesDelaySpin->setValue(core->mset.sub_delay);
    ui->applyButton->setEnabled(false);
}
