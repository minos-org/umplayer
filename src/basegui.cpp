/*  umplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2009 Ricardo Villalba <rvm@escomposlinux.org>
    Copyright (C) 2010 Ori Rejwan

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "basegui.h"

#include "filedialog.h"
#include <QMessageBox>
#include <QLabel>
#include <QMenu>
#include <QFileInfo>
#include <QApplication>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QCursor>
#include <QTimer>
#include <QStyle>
#include <QRegExp>
#include <QStatusBar>
#include <QActionGroup>
#include <QUrl>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDesktopServices>
#include <QInputDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QStyleFactory>
#include <QSysInfo>

#include <cmath>

#include "mplayerwindow.h"
#include "desktopinfo.h"
#include "helper.h"
#include "paths.h"
#include "colorutils.h"
#include "global.h"
#include "translator.h"
#include "images.h"
#include "preferences.h"
#include "discname.h"
#include "timeslider.h"
#include "logwindow.h"
#include "playlist.h"
#include "filepropertiesdialog.h"
#include "eqslider.h"
#include "videoequalizer.h"
#include "audioequalizer.h"
#include "equalizer.h"
#include "synchronization.h"
#include "inputdvddirectory.h"
#include "inputmplayerversion.h"
#include "inputurl.h"
#include "recents.h"
#include "urlhistory.h"
#include "about.h"
#include "errordialog.h"
#include "timedialog.h"
#include "clhelp.h"
#include "findsubtitleswindow.h"
#include "videopreview.h"
#include "mplayerversion.h"

#include "ytdialog.h"
#include "scdialog.h"
#include "config.h"
#include "actionseditor.h"

#include "myserver.h"

#include "tvlist.h"

#include "preferencesdialog.h"
#ifndef NO_USE_INI_FILES
#include "prefgeneral.h"
#endif
#include "prefinterface.h"
#include "prefinput.h"
#include "prefadvanced.h"

#include "myaction.h"
#include "myactiongroup.h"

#include "constants.h"

#include "extensions.h"
#include "qpropertysetter.h"
#include "youtubesearchbox.h"
#include "updatenotice.h"
#include "downloadprogressdialog.h"
#include "recordingdialog.h"
#include "version.h"
#include "languageselection.h"

#ifdef Q_OS_MACX
#include "globaldataclass.h"
#endif


#ifdef Q_OS_WIN
#include "deviceinfo.h"
#include "fileassocquestion.h"
#include "win7_include.h"
#endif

using namespace Global;

#ifdef Q_OS_MACX
GlobalDataClass globaldata;
#endif

BaseGui::BaseGui( QWidget* parent, Qt::WindowFlags flags )
	: QMainWindow( parent, flags ),
         floatingWidgetHeight(70),
		near_top(false),
                near_bottom(false)
{
#ifdef Q_OS_WIN
	/* Disable screensaver by event */
	just_stopped = false;
#endif
	ignore_show_hide_events = false;

        setWindowTitle( "UMPlayer" );

	// Not created objects
	server = 0;
	popup = 0;
	pref_dialog = 0;
	file_dialog = 0;
	clhelp_window = 0;
	find_subs_dialog = 0;
	video_preview = 0;
        // Create objects:
        createPanel();
        setCentralWidget(panel);

        createMplayerWindow();
	createCore();
        createPlaylist();
        createVideoEqualizer();
        createAudioEqualizer();
        createSynchronization();

        createEqualizer();

	// Mouse Wheel
	connect( this, SIGNAL(wheelUp()),
             core, SLOT(wheelUp()) );
	connect( this, SIGNAL(wheelDown()),
             core, SLOT(wheelDown()) );
	connect( mplayerwindow, SIGNAL(wheelUp()),
             core, SLOT(wheelUp()) );
	connect( mplayerwindow, SIGNAL(wheelDown()),
             core, SLOT(wheelDown()) );
        connect(RecordingDialog::getInstance(), SIGNAL(playFile(QString)), this, SLOT(remoteOpen(QString)));

	// Set style before changing color of widgets:
	// Set style
#if STYLE_SWITCHING
	qDebug( "Style name: '%s'", qApp->style()->objectName().toUtf8().data() );
	qDebug( "Style class name: '%s'", qApp->style()->metaObject()->className() );

	default_style = qApp->style()->objectName();
	if (!pref->style.isEmpty()) {
		qApp->setStyle( pref->style );
	}
#endif

    mplayer_log_window = new LogWindow(0);
        umplayer_log_window = new LogWindow(0);

	createActions();
        createMenus();
        createYTDialog();
#if AUTODISABLE_ACTIONS
	setActionsEnabled(false);
#endif

#if !DOCK_PLAYLIST
	connect(playlist, SIGNAL(visibilityChanged(bool)),
            showPlaylistAct, SLOT(setChecked(bool)) );
#endif

	retranslateStrings();

	setAcceptDrops(true);

	resize(pref->default_size);

        panel->setFocus();

	initializeGui();
        //testCheckUpdate();
        QTimer::singleShot(0, this, SLOT(showLanguageSelection()));
}



void BaseGui::createYTDialog()
{
        youTubeDialog = YTDialog::instance();
        connect(youTubeDialog, SIGNAL(gotUrls(QMap<int,QString>, QString, QString)), this, SLOT(playYTUrl(QMap<int,QString>,QString, QString)));
        shoutcastDialog = SCDialog::instance();
        connect(shoutcastDialog, SIGNAL(gotUrls(QString,QString)), this, SLOT(playSCUrl(QString,QString)));
}

void BaseGui::initializeGui() {
	changeStayOnTop(pref->stay_on_top);
	toggleFrameCounter( pref->show_frame_counter );

	updateRecents();

	// Call loadActions() outside initialization of the class.
	// Otherwise DefaultGui (and other subclasses) doesn't exist, and
	// its actions are not loaded
	QTimer::singleShot(20, this, SLOT(loadActions()));

	// Single instance
	server = new MyServer(this);
	connect(server, SIGNAL(receivedOpen(QString)),
            this, SLOT(remoteOpen(QString)));
	connect(server, SIGNAL(receivedOpenFiles(QStringList)),
            this, SLOT(remoteOpenFiles(QStringList)));
	connect(server, SIGNAL(receivedAddFiles(QStringList)),
            this, SLOT(remoteAddFiles(QStringList)));
	connect(server, SIGNAL(receivedFunction(QString)),
            this, SLOT(processFunction(QString)));
	connect(server, SIGNAL(receivedLoadSubtitle(QString)),
            this, SLOT(remoteLoadSubtitle(QString)));
        connect(server, SIGNAL(receivedAddDirectory(QString)),
                this, SLOT(remoteAddDirectory(QString)));
        connect(server, SIGNAL(receivedOpenDirectory(QString)),
                this, SLOT(remoteOpenDirectory(QString)));

	if (pref->use_single_instance) {
		int port = 0;
		if (!pref->use_autoport) port = pref->connection_port;
		if (server->listen(port)) {
			pref->autoport = server->serverPort();
			pref->save();
			qDebug("BaseGui::initializeGui: server running on port %d", pref->autoport);
		} else {
			qWarning("BaseGui::initializeGui: server couldn't be started");
		}
	}
        //if(pref->playCount <=10)
        //{
            //++pref->playCount;
            //pref->save();
        //}
        resizeFrameStep = new QTimer(this);
        resizeFrameStep->setInterval(300);
        connect(resizeFrameStep, SIGNAL(timeout()), this, SLOT(frameStepNeeded()));
        //if(pref->playCount == 10)
        //{
            //QTimer::singleShot(0, this, SLOT(facebookAlert()));
        //}
}

void BaseGui::frameStepNeeded()
{
    if(core->state() == Core::Paused)
    {
        frameStepAct->trigger();
    }
    resizeFrameStep->stop();
}

void BaseGui::testCheckUpdate()
{
    int elapsed = pref->lastUpdateTime.secsTo(QDateTime::currentDateTime());
    if(elapsed < 0 || elapsed > 432000)
    {
        checkUpdateNow();
    }
}

void BaseGui::checkUpdateNow()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest req(QUrl("http://update.umplayer.com"));
    QByteArray postData = QString("d=%1&g=%2&v=%3&o=%4").arg(pref->lastUpdateTime.toTime_t()).arg(pref->uid.toString().remove('{').remove('}')).arg(umplayerVersion()).arg(pref->os).toUtf8();
#ifdef PORTABLE_APP
    postData += "&portable=true";
#endif
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(updateNowResult(QNetworkReply*)));
    manager->post(req, postData);
}

void BaseGui::updateNowResult(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray replyString = reply->readAll();
        QList<QByteArray> params = replyString.split('&');
        QString a, u;
        foreach(QByteArray param, params )
        {
            if(param.startsWith("a="))
            {
                a = param.mid(2);
            }
            else if(param.startsWith("u="))
            {
                u = param.mid(2);
            }
        }
        if(!a.isEmpty())
        {
            askForUpdate(a, u);
        }
    }
    reply->manager()->deleteLater();
}

void BaseGui::askForUpdate(QString action, QString url)
{
    int act = action.toInt();
    if(act == 0)
    {
        changeUpdateDate();
    }
    else
    {
        UpdateNotice* notice = new UpdateNotice;
        notice->setUpdateType(act);
        int result = notice->exec();
        switch(act)
        {
        case 1:
        case 11:
            if(result != QDialog::Rejected)
                updateNow(url, act > 10);
            else
                changeUpdateDate();
            break;
        case 2:
        case 12:
            if(result != QDialog::Rejected)
                updateNow(url, act > 10);
            break;
        case 3:
        case 13:
            if(result != QDialog::Rejected)
                updateNow(url, act > 10);
            else
                closeWindow(true);
            break;
        }
    }
}

void BaseGui::updateNow(QString url, bool binary)
{
    if(!binary)
    {
        QDesktopServices::openUrl(url);
        changeUpdateDate();
        closeWindow(true);
    }
    else
    {
        DownloadBinaryDialog* d = new DownloadBinaryDialog(url);
        d->show();
        connect(d, SIGNAL(programStarted(bool)), this, SLOT(closeWindow(bool)));
    }
}

void BaseGui::changeUpdateDate()
{
    pref->lastUpdateTime = QDateTime::currentDateTime();
    pref->save();
}

void BaseGui::facebookAlert()
{
    connect(core, SIGNAL(stateChanged(Core::State)),
            this, SLOT(pauseIfNeeded(Core::State)));
    if(core->state() == Core::Playing)
    {
        core->pause();
    }
    QMessageBox::StandardButton button = QMessageBox::information(this, "Share UMPlayer with your friends",
                          "Would you like to share UMPlayer with your friends on Facebook?",
                             QMessageBox::Yes, QMessageBox::No);
    if(button == QMessageBox::Yes)
    {
        QDesktopServices::openUrl(QUrl("http://www.facebook.com/sharer.php?u=http%3A%2F%2Fwww.umplayer.com&t=UMPlayer+|+Universal+Multimedia+Player"));
    }
    else
    {
        if(core->state() == Core::Paused )
            core->play();
    }
    disconnect(core, SIGNAL(stateChanged(Core::State)),
            this, SLOT(pauseIfNeeded(Core::State)));

}

void BaseGui::pauseIfNeeded(Core::State state)
{
    if(state == Core::Playing )
    {
        core->pause();
    }
}

void BaseGui::showLanguageSelection()
{
    if(pref->language_is_set != true)
    {
        LanguageSelection* ls = new LanguageSelection(this);
        ls->exec();
    }
}

void BaseGui::remoteOpen(QString file) {
	qDebug("BaseGui::remoteOpen: '%s'", file.toUtf8().data());
	if (isMinimized()) showNormal();
	if (!isVisible()) show();
	raise();
	activateWindow();
	open(file);
}

void BaseGui::remoteOpenFiles(QStringList files) {
	qDebug("BaseGui::remoteOpenFiles");
	if (isMinimized()) showNormal();
	if (!isVisible()) show();
	raise();
	activateWindow();
	openFiles(files);
}

void BaseGui::remoteAddFiles(QStringList files) {
	qDebug("BaseGui::remoteAddFiles");
	if (isMinimized()) showNormal();
	if (!isVisible()) show();
	raise();
	activateWindow();
	playlist->addFiles(files);
	//open(files[0]);
}

void BaseGui::remoteLoadSubtitle(QString file) {
	qDebug("BaseGui::remoteLoadSubtitle: '%s'", file.toUtf8().data());

	setInitialSubtitle(file);

	if (core->state() != Core::Stopped) {
		core->loadSub(file);
	}
}

void BaseGui::remoteOpenDirectory(QString file)
{
        qDebug("BaseGui::remoteOpenDirectory");
        if (isMinimized()) showNormal();
        if (!isVisible()) show();
        raise();
        activateWindow();
        openDirectory(file);
}

void BaseGui::remoteAddDirectory(QString file)
{
        qDebug("BaseGui::remoteOpenDirectory");
        if (isMinimized()) showNormal();
        if (!isVisible()) show();
        raise();
        activateWindow();
        playlist->addDirectory(file);
}
BaseGui::~BaseGui() {
	delete core; // delete before mplayerwindow, otherwise, segfault...

	delete tvlist;
	delete radiolist;

//#if !DOCK_PLAYLIST
	if (playlist) {
		delete playlist;
		playlist = 0;
	}
//#endif

	if (find_subs_dialog) {
		delete find_subs_dialog;
		find_subs_dialog = 0; // Necessary?
	}

	if (video_preview) {
		delete video_preview;
	}
        delete mplayer_log_window;
        delete umplayer_log_window;

}

void BaseGui::createActions() {
	// Menu File
	openFileAct = new MyAction( QKeySequence("Ctrl+O"), this, "open_file" );
	connect( openFileAct, SIGNAL(triggered()),
             this, SLOT(openFile()) );
        IconSetter::instance()->openFileAct = openFileAct;
        openFileAct->change( Images::icon("file"), tr("&Open File...") );
	openDirectoryAct = new MyAction( this, "open_directory" );
	connect( openDirectoryAct, SIGNAL(triggered()),
             this, SLOT(openDirectory()) );
        IconSetter::instance()->openDirectoryAct = openDirectoryAct;
        openDirectoryAct->change( Images::icon("openfolder"), tr("D&irectory...") );

	openPlaylistAct = new MyAction( this, "open_playlist" );
	connect( openPlaylistAct, SIGNAL(triggered()),
             playlist, SLOT(load()) );

	openVCDAct = new MyAction( this, "open_vcd" );
	connect( openVCDAct, SIGNAL(triggered()),
             this, SLOT(openVCD()) );

	openAudioCDAct = new MyAction( this, "open_audio_cd" );
	connect( openAudioCDAct, SIGNAL(triggered()),
             this, SLOT(openAudioCD()) );

#ifdef Q_OS_WIN
	// VCD's and Audio CD's seem they don't work on windows
	//openVCDAct->setEnabled(pref->enable_vcd_on_windows);
	openAudioCDAct->setEnabled(pref->enable_audiocd_on_windows);
#endif

	openDVDAct = new MyAction( this, "open_dvd" );
	connect( openDVDAct, SIGNAL(triggered()),
             this, SLOT(openDVD()) );
        openDVDAct->change( Images::icon("dvd"), tr("&DVD from drive") );
        IconSetter::instance()->openDVDAct = openDVDAct ;


	openDVDFolderAct = new MyAction( this, "open_dvd_folder" );
	connect( openDVDFolderAct, SIGNAL(triggered()),
             this, SLOT(openDVDFromFolder()) );

	openURLAct = new MyAction( QKeySequence("Ctrl+U"), this, "open_url" );
	connect( openURLAct, SIGNAL(triggered()),
             this, SLOT(openURL()) );
        IconSetter::instance()->openURLAct = openURLAct;
        openURLAct->change( Images::icon("url"), tr("&URL...") );

	exitAct = new MyAction( QKeySequence("Ctrl+X"), this, "close" );
        connect( exitAct, SIGNAL(triggered()), this, SLOT(closeWindow()) );

	clearRecentsAct = new MyAction( this, "clear_recents" );
	connect( clearRecentsAct, SIGNAL(triggered()), this, SLOT(clearRecentsList()) );

	// TV and Radio
	tvlist = new TVList(pref->check_channels_conf_on_startup,
                        TVList::TV, Paths::configPath() + "/tv.m3u8", this);
	tvlist->menu()->menuAction()->setObjectName( "tv_menu" );
	addAction(tvlist->editAct());
	addAction(tvlist->jumpAct());
	addAction(tvlist->nextAct());
	addAction(tvlist->previousAct());
	tvlist->nextAct()->setShortcut( Qt::Key_H );
	tvlist->previousAct()->setShortcut( Qt::Key_L );
	tvlist->nextAct()->setObjectName("next_tv");
	tvlist->previousAct()->setObjectName("previous_tv");
	tvlist->editAct()->setObjectName("edit_tv_list");
	tvlist->jumpAct()->setObjectName("jump_tv_list");
	connect(tvlist, SIGNAL(activated(QString)), this, SLOT(open(QString)));

	radiolist = new TVList(pref->check_channels_conf_on_startup,
                           TVList::Radio, Paths::configPath() + "/radio.m3u8", this);
	radiolist->menu()->menuAction()->setObjectName( "radio_menu" );
	addAction(radiolist->editAct());
	addAction(radiolist->jumpAct());
	addAction(radiolist->nextAct());
	addAction(radiolist->previousAct());
	radiolist->nextAct()->setShortcut( Qt::SHIFT | Qt::Key_H );
	radiolist->previousAct()->setShortcut( Qt::SHIFT | Qt::Key_L );
	radiolist->nextAct()->setObjectName("next_radio");
	radiolist->previousAct()->setObjectName("previous_radio");
	radiolist->editAct()->setObjectName("edit_radio_list");
	radiolist->jumpAct()->setObjectName("jump_radio_list");
	connect(radiolist, SIGNAL(activated(QString)), this, SLOT(open(QString)));


	// Menu Play
	playAct = new MyAction( this, "play" );
	connect( playAct, SIGNAL(triggered()),
             core, SLOT(play()) );        

	playOrPauseAct = new MyAction( Qt::Key_MediaPlay, this, "play_or_pause" );
        connect( playOrPauseAct, SIGNAL(triggered(bool)), this, SLOT(play_or_pause(bool)) );
        playOrPauseAct->setCheckable(true);

	pauseAct = new MyAction( Qt::Key_Space, this, "pause" );
	connect( pauseAct, SIGNAL(triggered()),
             core, SLOT(pause()) );

	pauseAndStepAct = new MyAction( this, "pause_and_frame_step" );
	connect( pauseAndStepAct, SIGNAL(triggered()),
             core, SLOT(pause_and_frame_step()) );

	stopAct = new MyAction( Qt::Key_MediaStop, this, "stop" );
	connect( stopAct, SIGNAL(triggered()),
             core, SLOT(stop()) );

	frameStepAct = new MyAction( Qt::Key_Period, this, "frame_step" );
	connect( frameStepAct, SIGNAL(triggered()),
             core, SLOT(frameStep()) );

	rewind1Act = new MyAction( Qt::Key_Left, this, "rewind1" );
	connect( rewind1Act, SIGNAL(triggered()),
             core, SLOT(srewind()) );

        rewind2Act = new MyAction( this, "rewind2" );
	connect( rewind2Act, SIGNAL(triggered()),
             core, SLOT(rewind()) );

	rewind3Act = new MyAction( Qt::Key_PageDown, this, "rewind3" );
	connect( rewind3Act, SIGNAL(triggered()),
             core, SLOT(fastrewind()) );

	forward1Act = new MyAction( Qt::Key_Right, this, "forward1" );
	connect( forward1Act, SIGNAL(triggered()),
             core, SLOT(sforward()) );

        forward2Act = new MyAction( this, "forward2" );
	connect( forward2Act, SIGNAL(triggered()),
             core, SLOT(forward()) );

	forward3Act = new MyAction( Qt::Key_PageUp, this, "forward3" );
	connect( forward3Act, SIGNAL(triggered()),
             core, SLOT(fastforward()) );

	repeatAct = new MyAction( this, "repeat" );
	repeatAct->setCheckable( true );
	connect( repeatAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleRepeat(bool)) );

	gotoAct = new MyAction( QKeySequence("Ctrl+J"), this, "jump_to" );
	connect( gotoAct, SIGNAL(triggered()),
             this, SLOT(showGotoDialog()) );

	// Submenu Speed
	normalSpeedAct = new MyAction( Qt::Key_Backspace, this, "normal_speed" );
	connect( normalSpeedAct, SIGNAL(triggered()),
             core, SLOT(normalSpeed()) );

	halveSpeedAct = new MyAction( Qt::Key_BraceLeft, this, "halve_speed" );
	connect( halveSpeedAct, SIGNAL(triggered()),
             core, SLOT(halveSpeed()) );

	doubleSpeedAct = new MyAction( Qt::Key_BraceRight, this, "double_speed" );
	connect( doubleSpeedAct, SIGNAL(triggered()),
             core, SLOT(doubleSpeed()) );

	decSpeed10Act = new MyAction( Qt::Key_BracketLeft, this, "dec_speed" );
	connect( decSpeed10Act, SIGNAL(triggered()),
             core, SLOT(decSpeed10()) );

	incSpeed10Act = new MyAction( Qt::Key_BracketRight, this, "inc_speed" );
	connect( incSpeed10Act, SIGNAL(triggered()),
             core, SLOT(incSpeed10()) );

	decSpeed4Act = new MyAction( this, "dec_speed_4" );
	connect( decSpeed4Act, SIGNAL(triggered()),
             core, SLOT(decSpeed4()) );

	incSpeed4Act = new MyAction( this, "inc_speed_4" );
	connect( incSpeed4Act, SIGNAL(triggered()),
             core, SLOT(incSpeed4()) );

	decSpeed1Act = new MyAction( this, "dec_speed_1" );
	connect( decSpeed1Act, SIGNAL(triggered()),
             core, SLOT(decSpeed1()) );

	incSpeed1Act = new MyAction( this, "inc_speed_1" );
	connect( incSpeed1Act, SIGNAL(triggered()),
             core, SLOT(incSpeed1()) );


	// Menu Video
	fullscreenAct = new MyAction( Qt::Key_F, this, "fullscreen" );
	fullscreenAct->setCheckable( true );
	connect( fullscreenAct, SIGNAL(toggled(bool)),
             this, SLOT(toggleFullscreen(bool)) );
        restoreAct = new MyAction( this, "restore" );
        connect( restoreAct, SIGNAL(triggered(bool)),
             this, SLOT(restore()) );

	videoEqualizerAct = new MyAction( QKeySequence("Ctrl+E"), this, "video_equalizer" );	
        connect( videoEqualizerAct, SIGNAL(triggered()),
             this, SLOT(showVideoEqualizer()) );

	// Single screenshot
	screenshotAct = new MyAction( Qt::Key_S, this, "screenshot" );
	connect( screenshotAct, SIGNAL(triggered()),
             core, SLOT(screenshot()) );
        screenshotAct->change( Images::icon("screenshot"), tr("&Screenshot") );
        IconSetter::instance()->screenshotAct = screenshotAct;

        // Multiple screenshots
        screenshotsAct = new MyAction( QKeySequence("Shift+D"), this, "multiple_screenshots" );
	connect( screenshotsAct, SIGNAL(triggered()),
             core, SLOT(screenshots()) );

	videoPreviewAct = new MyAction( this, "video_preview" );
	connect( videoPreviewAct, SIGNAL(triggered()),
             this, SLOT(showVideoPreviewDialog()) );

	flipAct = new MyAction( this, "flip" );
	flipAct->setCheckable( true );
	connect( flipAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleFlip(bool)) );

	mirrorAct = new MyAction( this, "mirror" );
	mirrorAct->setCheckable( true );
	connect( mirrorAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleMirror(bool)) );

	motionVectorsAct = new MyAction( this, "motion_vectors" );
	motionVectorsAct->setCheckable( true );
	connect( motionVectorsAct, SIGNAL(toggled(bool)),
             core, SLOT(visualizeMotionVectors(bool)) );


	// Submenu filter
	postProcessingAct = new MyAction( this, "postprocessing" );
	postProcessingAct->setCheckable( true );
	connect( postProcessingAct, SIGNAL(toggled(bool)),
             core, SLOT(togglePostprocessing(bool)) );

	phaseAct = new MyAction( this, "autodetect_phase" );
	phaseAct->setCheckable( true );
	connect( phaseAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleAutophase(bool)) );

	deblockAct = new MyAction( this, "deblock" );
	deblockAct->setCheckable( true );
	connect( deblockAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleDeblock(bool)) );

	deringAct = new MyAction( this, "dering" );
	deringAct->setCheckable( true );
	connect( deringAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleDering(bool)) );

	addNoiseAct = new MyAction( this, "add_noise" );
	addNoiseAct->setCheckable( true );
	connect( addNoiseAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleNoise(bool)) );

	addLetterboxAct = new MyAction( this, "add_letterbox" );
	addLetterboxAct->setCheckable( true );
	connect( addLetterboxAct, SIGNAL(toggled(bool)),
             core, SLOT(changeLetterbox(bool)) );

	upscaleAct = new MyAction( this, "upscaling" );
	upscaleAct->setCheckable( true );
	connect( upscaleAct, SIGNAL(toggled(bool)),
             core, SLOT(changeUpscale(bool)) );


	// Menu Audio
	audioEqualizerAct = new MyAction( this, "audio_equalizer" );	
        connect( audioEqualizerAct, SIGNAL(triggered()),
             this, SLOT(showAudioEqualizer()) );

	muteAct = new MyAction( Qt::Key_M, this, "mute" );
	muteAct->setCheckable( true );
	connect( muteAct, SIGNAL(toggled(bool)),
             core, SLOT(mute(bool)) );

#if USE_MULTIPLE_SHORTCUTS
	decVolumeAct = new MyAction( this, "decrease_volume" );
        decVolumeAct->setShortcuts( ActionsEditor::stringToShortcuts("Down,9,/") );
#else
        decVolumeAct = new MyAction( Qt::Key_Down, this, "dec_volume" );
#endif
	connect( decVolumeAct, SIGNAL(triggered()),
             core, SLOT(decVolume()) );

#if USE_MULTIPLE_SHORTCUTS
	incVolumeAct = new MyAction( this, "increase_volume" );
        incVolumeAct->setShortcuts( ActionsEditor::stringToShortcuts("Up,0,*") );
#else
        incVolumeAct = new MyAction( Qt::Key_Up, this, "inc_volume" );
#endif
	connect( incVolumeAct, SIGNAL(triggered()),
             core, SLOT(incVolume()) );

	decAudioDelayAct = new MyAction( Qt::Key_Minus, this, "dec_audio_delay" );
	connect( decAudioDelayAct, SIGNAL(triggered()),
             core, SLOT(decAudioDelay()) );

	incAudioDelayAct = new MyAction( Qt::Key_Plus, this, "inc_audio_delay" );
	connect( incAudioDelayAct, SIGNAL(triggered()),
             core, SLOT(incAudioDelay()) );

	audioDelayAct = new MyAction( this, "audio_delay" );
	connect( audioDelayAct, SIGNAL(triggered()),
             this, SLOT(showAudioDelayDialog()) );

	loadAudioAct = new MyAction( this, "load_audio_file" );
	connect( loadAudioAct, SIGNAL(triggered()),
             this, SLOT(loadAudioFile()) );

	unloadAudioAct = new MyAction( this, "unload_audio_file" );
	connect( unloadAudioAct, SIGNAL(triggered()),
             core, SLOT(unloadAudioFile()) );


	// Submenu Filters
	extrastereoAct = new MyAction( this, "extrastereo_filter" );
	extrastereoAct->setCheckable( true );
	connect( extrastereoAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleExtrastereo(bool)) );

	karaokeAct = new MyAction( this, "karaoke_filter" );
	karaokeAct->setCheckable( true );
	connect( karaokeAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleKaraoke(bool)) );

	volnormAct = new MyAction( this, "volnorm_filter" );
	volnormAct->setCheckable( true );
	connect( volnormAct, SIGNAL(toggled(bool)),
             core, SLOT(toggleVolnorm(bool)) );


	// Menu Subtitles
	loadSubsAct = new MyAction( this, "load_subs" );
	connect( loadSubsAct, SIGNAL(triggered()),
             this, SLOT(loadSub()) );

	unloadSubsAct = new MyAction( this, "unload_subs" );
	connect( unloadSubsAct, SIGNAL(triggered()),
             core, SLOT(unloadSub()) );

	decSubDelayAct = new MyAction( Qt::Key_Z, this, "dec_sub_delay" );
	connect( decSubDelayAct, SIGNAL(triggered()),
             core, SLOT(decSubDelay()) );

	incSubDelayAct = new MyAction( Qt::Key_X, this, "inc_sub_delay" );
	connect( incSubDelayAct, SIGNAL(triggered()),
             core, SLOT(incSubDelay()) );

	subDelayAct = new MyAction( this, "sub_delay" );
	connect( subDelayAct, SIGNAL(triggered()),
             this, SLOT(showSubDelayDialog()) );

	decSubPosAct = new MyAction( Qt::Key_R, this, "dec_sub_pos" );
	connect( decSubPosAct, SIGNAL(triggered()),
             core, SLOT(decSubPos()) );
	incSubPosAct = new MyAction( Qt::Key_T, this, "inc_sub_pos" );
	connect( incSubPosAct, SIGNAL(triggered()),
             core, SLOT(incSubPos()) );

	decSubScaleAct = new MyAction( Qt::SHIFT | Qt::Key_R, this, "dec_sub_scale" );
	connect( decSubScaleAct, SIGNAL(triggered()),
             core, SLOT(decSubScale()) );

	incSubScaleAct = new MyAction( Qt::SHIFT | Qt::Key_T, this, "inc_sub_scale" );
	connect( incSubScaleAct, SIGNAL(triggered()),
             core, SLOT(incSubScale()) );

	decSubStepAct = new MyAction( Qt::Key_G, this, "dec_sub_step" );
	connect( decSubStepAct, SIGNAL(triggered()),
             core, SLOT(decSubStep()) );

	incSubStepAct = new MyAction( Qt::Key_Y, this, "inc_sub_step" );
	connect( incSubStepAct, SIGNAL(triggered()),
             core, SLOT(incSubStep()) );

	useAssAct = new MyAction(this, "use_ass_lib");
	useAssAct->setCheckable(true);
	connect( useAssAct, SIGNAL(toggled(bool)), core, SLOT(changeUseAss(bool)) );

	useClosedCaptionAct = new MyAction(this, "use_closed_caption");
	useClosedCaptionAct->setCheckable(true);
	connect( useClosedCaptionAct, SIGNAL(toggled(bool)), core, SLOT(toggleClosedCaption(bool)) );

	useForcedSubsOnlyAct = new MyAction(this, "use_forced_subs_only");
	useForcedSubsOnlyAct->setCheckable(true);
	connect( useForcedSubsOnlyAct, SIGNAL(toggled(bool)), core, SLOT(toggleForcedSubsOnly(bool)) );

	subVisibilityAct = new MyAction(Qt::Key_V, this, "subtitle_visibility");
	subVisibilityAct->setCheckable(true);
	connect( subVisibilityAct, SIGNAL(toggled(bool)), core, SLOT(changeSubVisibility(bool)) );

	showFindSubtitlesDialogAct = new MyAction( this, "show_find_sub_dialog" );
	connect( showFindSubtitlesDialogAct, SIGNAL(triggered()),
             this, SLOT(showFindSubtitlesDialog()) );

        IconSetter::instance()->showFindSubtitlesDialogAct = showFindSubtitlesDialogAct;
        showFindSubtitlesDialogAct->change( Images::icon("download_subs"), tr("&Open File...") );

	openUploadSubtitlesPageAct = new MyAction( this, "upload_subtitles" );		//turbos
	connect( openUploadSubtitlesPageAct, SIGNAL(triggered()),					//turbos
             this, SLOT(openUploadSubtitlesPage()) );							//turbos


	// Menu Options
        showRecordingsAct = new MyAction(QKeySequence("Ctrl+R"), this, "show_recordings");
        connect( showRecordingsAct, SIGNAL(triggered()), this, SLOT(showRecordings()));
	showPlaylistAct = new MyAction( QKeySequence("Ctrl+L"), this, "show_playlist" );
	showPlaylistAct->setCheckable( true );
	connect( showPlaylistAct, SIGNAL(toggled(bool)),
             this, SLOT(showPlaylist(bool)) );

	showPropertiesAct = new MyAction( QKeySequence("Ctrl+I"), this, "show_file_properties" );
	connect( showPropertiesAct, SIGNAL(triggered()),
             this, SLOT(showFilePropertiesDialog()) );
        showPropertiesAct->change( Images::icon("info"), tr("View &info and properties...") );
        IconSetter::instance()->showPropertiesAct = showPropertiesAct;

	frameCounterAct = new MyAction( this, "frame_counter" );
	frameCounterAct->setCheckable( true );
	connect( frameCounterAct, SIGNAL(toggled(bool)),
             this, SLOT(toggleFrameCounter(bool)) );

	showPreferencesAct = new MyAction( QKeySequence("Ctrl+P"), this, "show_preferences" );
	connect( showPreferencesAct, SIGNAL(triggered()),
             this, SLOT(showPreferencesDialog()) );
        showPreferencesAct->change( Images::icon("prefs"), tr("P&references") );
        IconSetter::instance()->showPreferencesAct = showPreferencesAct ;

	// Submenu Logs
	showLogMplayerAct = new MyAction( QKeySequence("Ctrl+M"), this, "show_mplayer_log" );
	connect( showLogMplayerAct, SIGNAL(triggered()),
             this, SLOT(showMplayerLog()) );

        showLogSmplayerAct = new MyAction( QKeySequence("Ctrl+S"), this, "show_umplayer_log" );
	connect( showLogSmplayerAct, SIGNAL(triggered()),
             this, SLOT(showLog()) );

	// Menu Help
	showFAQAct = new MyAction( this, "faq" );
	connect( showFAQAct, SIGNAL(triggered()),
             this, SLOT(helpFAQ()) );

	showCLOptionsAct = new MyAction( this, "cl_options" );
	connect( showCLOptionsAct, SIGNAL(triggered()),
             this, SLOT(helpCLOptions()) );
        homepageAct = new MyAction( this, "homepage" );
        connect( homepageAct, SIGNAL(triggered()),
             this, SLOT(helpHomepage()));

	showTipsAct = new MyAction( this, "tips" );
	connect( showTipsAct, SIGNAL(triggered()),
             this, SLOT(helpTips()) );

        facebookAct = new MyAction (this, "facebook");
        twitterAct = new MyAction (this, "twitter");
        mySpaceAct = new MyAction (this, "myspace");
        gmailAct = new MyAction (this, "gmail");
        hotmailAct = new MyAction (this, "hotmail");
        yahooAct = new MyAction (this, "yahoo");

        connect( facebookAct, SIGNAL(triggered()),
             this, SLOT(shareUmplayer()));
        connect( twitterAct, SIGNAL(triggered()),
             this, SLOT(shareUmplayer()));
        connect( mySpaceAct, SIGNAL(triggered()),
             this, SLOT(shareUmplayer()));
        connect( gmailAct, SIGNAL(triggered()),
             this, SLOT(shareUmplayer()));
        connect( hotmailAct, SIGNAL(triggered()),
             this, SLOT(shareUmplayer()));
        connect( yahooAct, SIGNAL(triggered()),
             this, SLOT(shareUmplayer()));


	aboutQtAct = new MyAction( this, "about_qt" );
	connect( aboutQtAct, SIGNAL(triggered()),
             this, SLOT(helpAboutQt()) );

        aboutThisAct = new MyAction( this, "about_umplayer" );
	connect( aboutThisAct, SIGNAL(triggered()),
             this, SLOT(helpAbout()) );

	// Playlist
	playNextAct = new MyAction(Qt::Key_Greater, this, "play_next");
	connect( playNextAct, SIGNAL(triggered()), playlist, SLOT(playNext()) );

	playPrevAct = new MyAction(Qt::Key_Less, this, "play_prev");
	connect( playPrevAct, SIGNAL(triggered()), playlist, SLOT(playPrev()) );

        shuffleAct = new MyAction(QKeySequence("Ctrl+Alt+S"), this, "shuffle" );
        shuffleAct->setCheckable(true);
        connect( shuffleAct, SIGNAL(triggered(bool)), playlist, SLOT(toggleShuffle(bool)));

        repeatPlaylistAct= new MyAction(QKeySequence("Ctrl+Alt+P"), this, "repeat_playlist");
        repeatPlaylistAct->setCheckable(true);
        connect(repeatPlaylistAct, SIGNAL(triggered(bool)), playlist, SLOT(toggleRepeat(bool)));




	// Move video window and zoom
	moveUpAct = new MyAction(Qt::ALT | Qt::Key_Up, this, "move_up");
	connect( moveUpAct, SIGNAL(triggered()), mplayerwindow, SLOT(moveUp()) );

	moveDownAct = new MyAction(Qt::ALT | Qt::Key_Down, this, "move_down");
	connect( moveDownAct, SIGNAL(triggered()), mplayerwindow, SLOT(moveDown()) );

	moveLeftAct = new MyAction(Qt::ALT | Qt::Key_Left, this, "move_left");
	connect( moveLeftAct, SIGNAL(triggered()), mplayerwindow, SLOT(moveLeft()) );

	moveRightAct = new MyAction(Qt::ALT | Qt::Key_Right, this, "move_right");
	connect( moveRightAct, SIGNAL(triggered()), mplayerwindow, SLOT(moveRight()) );

	incZoomAct = new MyAction(Qt::Key_E, this, "inc_zoom");
	connect( incZoomAct, SIGNAL(triggered()), core, SLOT(incPanscan()) );

	decZoomAct = new MyAction(Qt::Key_W, this, "dec_zoom");
	connect( decZoomAct, SIGNAL(triggered()), core, SLOT(decPanscan()) );

	resetZoomAct = new MyAction(Qt::SHIFT | Qt::Key_E, this, "reset_zoom");
	connect( resetZoomAct, SIGNAL(triggered()), core, SLOT(resetPanscan()) );

	autoZoomAct = new MyAction(Qt::SHIFT | Qt::Key_W, this, "auto_zoom");
	connect( autoZoomAct, SIGNAL(triggered()), core, SLOT(autoPanscan()) );

	autoZoom169Act = new MyAction(Qt::SHIFT | Qt::Key_A, this, "zoom_169");
	connect( autoZoom169Act, SIGNAL(triggered()), core, SLOT(autoPanscanFor169()) );

	autoZoom235Act = new MyAction(Qt::SHIFT | Qt::Key_S, this, "zoom_235");
	connect( autoZoom235Act, SIGNAL(triggered()), core, SLOT(autoPanscanFor235()) );


	// Actions not in menus or buttons
	// Volume 2
#if !USE_MULTIPLE_SHORTCUTS
	decVolume2Act = new MyAction( Qt::Key_Slash, this, "dec_volume2" );
	connect( decVolume2Act, SIGNAL(triggered()), core, SLOT(decVolume()) );

	incVolume2Act = new MyAction( Qt::Key_Asterisk, this, "inc_volume2" );
	connect( incVolume2Act, SIGNAL(triggered()), core, SLOT(incVolume()) );
#endif
	// Exit fullscreen
	exitFullscreenAct = new MyAction( Qt::Key_Escape, this, "exit_fullscreen" );
	connect( exitFullscreenAct, SIGNAL(triggered()), this, SLOT(exitFullscreen()) );        

	nextOSDAct = new MyAction( Qt::Key_O, this, "next_osd");
	connect( nextOSDAct, SIGNAL(triggered()), core, SLOT(nextOSD()) );

	decContrastAct = new MyAction( Qt::Key_1, this, "dec_contrast");
	connect( decContrastAct, SIGNAL(triggered()), core, SLOT(decContrast()) );

	incContrastAct = new MyAction( Qt::Key_2, this, "inc_contrast");
	connect( incContrastAct, SIGNAL(triggered()), core, SLOT(incContrast()) );

	decBrightnessAct = new MyAction( Qt::Key_3, this, "dec_brightness");
	connect( decBrightnessAct, SIGNAL(triggered()), core, SLOT(decBrightness()) );

	incBrightnessAct = new MyAction( Qt::Key_4, this, "inc_brightness");
	connect( incBrightnessAct, SIGNAL(triggered()), core, SLOT(incBrightness()) );

	decHueAct = new MyAction(Qt::Key_5, this, "dec_hue");
	connect( decHueAct, SIGNAL(triggered()), core, SLOT(decHue()) );

	incHueAct = new MyAction( Qt::Key_6, this, "inc_hue");
	connect( incHueAct, SIGNAL(triggered()), core, SLOT(incHue()) );

	decSaturationAct = new MyAction( Qt::Key_7, this, "dec_saturation");
	connect( decSaturationAct, SIGNAL(triggered()), core, SLOT(decSaturation()) );

	incSaturationAct = new MyAction( Qt::Key_8, this, "inc_saturation");
	connect( incSaturationAct, SIGNAL(triggered()), core, SLOT(incSaturation()) );

	decGammaAct = new MyAction( this, "dec_gamma");
	connect( decGammaAct, SIGNAL(triggered()), core, SLOT(decGamma()) );

	incGammaAct = new MyAction( this, "inc_gamma");
	connect( incGammaAct, SIGNAL(triggered()), core, SLOT(incGamma()) );

	nextVideoAct = new MyAction( this, "next_video");
	connect( nextVideoAct, SIGNAL(triggered()), core, SLOT(nextVideo()) );

	nextAudioAct = new MyAction( Qt::Key_K, this, "next_audio");
	connect( nextAudioAct, SIGNAL(triggered()), core, SLOT(nextAudio()) );

	nextSubtitleAct = new MyAction( Qt::Key_J, this, "next_subtitle");
	connect( nextSubtitleAct, SIGNAL(triggered()), core, SLOT(nextSubtitle()) );

	nextChapterAct = new MyAction( Qt::Key_At, this, "next_chapter");
	connect( nextChapterAct, SIGNAL(triggered()), core, SLOT(nextChapter()) );

	prevChapterAct = new MyAction( Qt::Key_Exclam, this, "prev_chapter");
	connect( prevChapterAct, SIGNAL(triggered()), core, SLOT(prevChapter()) );

	doubleSizeAct = new MyAction( Qt::CTRL | Qt::Key_D, this, "toggle_double_size");
	connect( doubleSizeAct, SIGNAL(triggered()), core, SLOT(toggleDoubleSize()) );

	resetVideoEqualizerAct = new MyAction( this, "reset_video_equalizer");
	connect( resetVideoEqualizerAct, SIGNAL(triggered()), video_equalizer, SLOT(reset()) );

	resetAudioEqualizerAct = new MyAction( this, "reset_audio_equalizer");
	connect( resetAudioEqualizerAct, SIGNAL(triggered()), audio_equalizer, SLOT(reset()) );

	showContextMenuAct = new MyAction( this, "show_context_menu");
	connect( showContextMenuAct, SIGNAL(triggered()),
             this, SLOT(showPopupMenu()) );

	nextAspectAct = new MyAction( Qt::Key_A, this, "next_aspect");
	connect( nextAspectAct, SIGNAL(triggered()),
             core, SLOT(nextAspectRatio()) );

	nextWheelFunctionAct = new MyAction(this, "next_wheel_function");
	connect( nextWheelFunctionAct, SIGNAL(triggered()),
			 core, SLOT(nextWheelFunction()) );

	// Group actions

	// OSD
	osdGroup = new MyActionGroup(this);
	osdNoneAct = new MyActionGroupItem(this, osdGroup, "osd_none", Preferences::None);
	osdSeekAct = new MyActionGroupItem(this, osdGroup, "osd_seek", Preferences::Seek);
	osdTimerAct = new MyActionGroupItem(this, osdGroup, "osd_timer", Preferences::SeekTimer);
	osdTotalAct = new MyActionGroupItem(this, osdGroup, "osd_total", Preferences::SeekTimerTotal);
	connect( osdGroup, SIGNAL(activated(int)), core, SLOT(changeOSD(int)) );

	// Denoise
	denoiseGroup = new MyActionGroup(this);
	denoiseNoneAct = new MyActionGroupItem(this, denoiseGroup, "denoise_none", MediaSettings::NoDenoise);
	denoiseNormalAct = new MyActionGroupItem(this, denoiseGroup, "denoise_normal", MediaSettings::DenoiseNormal);
	denoiseSoftAct = new MyActionGroupItem(this, denoiseGroup, "denoise_soft", MediaSettings::DenoiseSoft);
	connect( denoiseGroup, SIGNAL(activated(int)), core, SLOT(changeDenoise(int)) );

	// Video size
	sizeGroup = new MyActionGroup(this);
	size50 = new MyActionGroupItem(this, sizeGroup, "5&0%", "size_50", 50);
	size75 = new MyActionGroupItem(this, sizeGroup, "7&5%", "size_75", 75);
	size100 = new MyActionGroupItem(this, sizeGroup, "&100%", "size_100", 100);
	size125 = new MyActionGroupItem(this, sizeGroup, "1&25%", "size_125", 125);
	size150 = new MyActionGroupItem(this, sizeGroup, "15&0%", "size_150", 150);
	size175 = new MyActionGroupItem(this, sizeGroup, "1&75%", "size_175", 175);
	size200 = new MyActionGroupItem(this, sizeGroup, "&200%", "size_200", 200);
	size300 = new MyActionGroupItem(this, sizeGroup, "&300%", "size_300", 300);
	size400 = new MyActionGroupItem(this, sizeGroup, "&400%", "size_400", 400);
	size100->setShortcut( Qt::CTRL | Qt::Key_1 );
	size200->setShortcut( Qt::CTRL | Qt::Key_2 );
	connect( sizeGroup, SIGNAL(activated(int)), core, SLOT(changeSize(int)) );
	// Make all not checkable
	QList <QAction *> size_list = sizeGroup->actions();
	for (int n=0; n < size_list.count(); n++) {
		size_list[n]->setCheckable(false);
	}

	// Deinterlace
	deinterlaceGroup = new MyActionGroup(this);
	deinterlaceNoneAct = new MyActionGroupItem(this, deinterlaceGroup, "deinterlace_none", MediaSettings::NoDeinterlace);
	deinterlaceL5Act = new MyActionGroupItem(this, deinterlaceGroup, "deinterlace_l5", MediaSettings::L5);
	deinterlaceYadif0Act = new MyActionGroupItem(this, deinterlaceGroup, "deinterlace_yadif0", MediaSettings::Yadif);
	deinterlaceYadif1Act = new MyActionGroupItem(this, deinterlaceGroup, "deinterlace_yadif1", MediaSettings::Yadif_1);
	deinterlaceLBAct = new MyActionGroupItem(this, deinterlaceGroup, "deinterlace_lb", MediaSettings::LB);
	deinterlaceKernAct = new MyActionGroupItem(this, deinterlaceGroup, "deinterlace_kern", MediaSettings::Kerndeint);
	connect( deinterlaceGroup, SIGNAL(activated(int)),
             core, SLOT(changeDeinterlace(int)) );

	// Audio channels
	channelsGroup = new MyActionGroup(this);
	/* channelsDefaultAct = new MyActionGroupItem(this, channelsGroup, "channels_default", MediaSettings::ChDefault); */
	channelsStereoAct = new MyActionGroupItem(this, channelsGroup, "channels_stereo", MediaSettings::ChStereo);
	channelsSurroundAct = new MyActionGroupItem(this, channelsGroup, "channels_surround", MediaSettings::ChSurround);
	channelsFull51Act = new MyActionGroupItem(this, channelsGroup, "channels_ful51", MediaSettings::ChFull51);
	connect( channelsGroup, SIGNAL(activated(int)),
             core, SLOT(setAudioChannels(int)) );

	// Stereo mode
	stereoGroup = new MyActionGroup(this);
	stereoAct = new MyActionGroupItem(this, stereoGroup, "stereo", MediaSettings::Stereo);
	leftChannelAct = new MyActionGroupItem(this, stereoGroup, "left_channel", MediaSettings::Left);
	rightChannelAct = new MyActionGroupItem(this, stereoGroup, "right_channel", MediaSettings::Right);
	connect( stereoGroup, SIGNAL(activated(int)),
             core, SLOT(setStereoMode(int)) );

	// Video aspect
	aspectGroup = new MyActionGroup(this);
	aspectDetectAct = new MyActionGroupItem(this, aspectGroup, "aspect_detect", MediaSettings::AspectAuto);
	aspect11Act = new MyActionGroupItem(this, aspectGroup, "aspect_1:1", MediaSettings::Aspect11 );
	aspect32Act = new MyActionGroupItem(this, aspectGroup, "aspect_3:2", MediaSettings::Aspect32);
	aspect43Act = new MyActionGroupItem(this, aspectGroup, "aspect_4:3", MediaSettings::Aspect43);
	aspect54Act = new MyActionGroupItem(this, aspectGroup, "aspect_5:4", MediaSettings::Aspect54 );
	aspect149Act = new MyActionGroupItem(this, aspectGroup, "aspect_14:9", MediaSettings::Aspect149 );
	aspect1410Act = new MyActionGroupItem(this, aspectGroup, "aspect_14:10", MediaSettings::Aspect1410 );
	aspect169Act = new MyActionGroupItem(this, aspectGroup, "aspect_16:9", MediaSettings::Aspect169 );
	aspect1610Act = new MyActionGroupItem(this, aspectGroup, "aspect_16:10", MediaSettings::Aspect1610 );
	aspect235Act = new MyActionGroupItem(this, aspectGroup, "aspect_2.35:1", MediaSettings::Aspect235 );
	{
		QAction * sep = new QAction(aspectGroup);
		sep->setSeparator(true);
	}
	aspectNoneAct = new MyActionGroupItem(this, aspectGroup, "aspect_none", MediaSettings::AspectNone);

	connect( aspectGroup, SIGNAL(activated(int)),
             core, SLOT(changeAspectRatio(int)) );

	// Rotate
	rotateGroup = new MyActionGroup(this);
	rotateNoneAct = new MyActionGroupItem(this, rotateGroup, "rotate_none", MediaSettings::NoRotate);
	rotateClockwiseFlipAct = new MyActionGroupItem(this, rotateGroup, "rotate_clockwise_flip", MediaSettings::Clockwise_flip);
	rotateClockwiseAct = new MyActionGroupItem(this, rotateGroup, "rotate_clockwise", MediaSettings::Clockwise);
	rotateCounterclockwiseAct = new MyActionGroupItem(this, rotateGroup, "rotate_counterclockwise", MediaSettings::Counterclockwise);
	rotateCounterclockwiseFlipAct = new MyActionGroupItem(this, rotateGroup, "rotate_counterclockwise_flip", MediaSettings::Counterclockwise_flip);
	connect( rotateGroup, SIGNAL(activated(int)),
             core, SLOT(changeRotate(int)) );

	// On Top
	onTopActionGroup = new MyActionGroup(this);
	onTopAlwaysAct = new MyActionGroupItem( this,onTopActionGroup,"on_top_always",Preferences::AlwaysOnTop);
	onTopNeverAct = new MyActionGroupItem( this,onTopActionGroup,"on_top_never",Preferences::NeverOnTop);
	onTopWhilePlayingAct = new MyActionGroupItem( this,onTopActionGroup,"on_top_playing",Preferences::WhilePlayingOnTop);
	connect( onTopActionGroup , SIGNAL(activated(int)),
             this, SLOT(changeStayOnTop(int)) );

	toggleStayOnTopAct = new MyAction( this, "toggle_stay_on_top");
	connect( toggleStayOnTopAct, SIGNAL(triggered()), this, SLOT(toggleStayOnTop()) );


#if USE_ADAPTER
	screenGroup = new MyActionGroup(this);
	screenDefaultAct = new MyActionGroupItem(this, screenGroup, "screen_default", -1);
#ifdef Q_OS_WIN
	DeviceList display_devices = DeviceInfo::displayDevices();
	if (!display_devices.isEmpty()) {
		for (int n = 0; n < display_devices.count(); n++) {
			int id = display_devices[n].ID().toInt();
			QString desc = display_devices[n].desc();
			MyAction * screen_item = new MyActionGroupItem(this, screenGroup, QString("screen_%1").arg(n).toAscii().constData(), id);
			screen_item->change( "&"+QString::number(n) + " - " + desc);
		}
	}
	else
#endif // Q_OS_WIN
	for (int n = 1; n <= 4; n++) {
		MyAction * screen_item = new MyActionGroupItem(this, screenGroup, QString("screen_%1").arg(n).toAscii().constData(), n);
		screen_item->change( "&"+QString::number(n) );
	}

	connect( screenGroup, SIGNAL(activated(int)),
             core, SLOT(changeAdapter(int)) );
#endif

#if PROGRAM_SWITCH
	// Program track
	programTrackGroup = new MyActionGroup(this);
	connect( programTrackGroup, SIGNAL(activated(int)),
	         core, SLOT(changeProgram(int)) );
#endif

	// Video track
	videoTrackGroup = new MyActionGroup(this);
	connect( videoTrackGroup, SIGNAL(activated(int)),
	         core, SLOT(changeVideo(int)) );

	// Audio track
	audioTrackGroup = new MyActionGroup(this);
	connect( audioTrackGroup, SIGNAL(activated(int)),
	         core, SLOT(changeAudio(int)) );

	// Subtitle track
	subtitleTrackGroup = new MyActionGroup(this);
    connect( subtitleTrackGroup, SIGNAL(activated(int)),
	         core, SLOT(changeSubtitle(int)) );

	// Titles
	titleGroup = new MyActionGroup(this);
	connect( titleGroup, SIGNAL(activated(int)),
			 core, SLOT(changeTitle(int)) );

	// Angles
	angleGroup = new MyActionGroup(this);
	connect( angleGroup, SIGNAL(activated(int)),
			 core, SLOT(changeAngle(int)) );

	// Chapters
	chapterGroup = new MyActionGroup(this);
	connect( chapterGroup, SIGNAL(activated(int)),
			 core, SLOT(changeChapter(int)) );

#if DVDNAV_SUPPORT
	dvdnavUpAct = new MyAction(Qt::SHIFT | Qt::Key_Up, this, "dvdnav_up");
	connect( dvdnavUpAct, SIGNAL(triggered()), core, SLOT(dvdnavUp()) );

	dvdnavDownAct = new MyAction(Qt::SHIFT | Qt::Key_Down, this, "dvdnav_down");
	connect( dvdnavDownAct, SIGNAL(triggered()), core, SLOT(dvdnavDown()) );

	dvdnavLeftAct = new MyAction(Qt::SHIFT | Qt::Key_Left, this, "dvdnav_left");
	connect( dvdnavLeftAct, SIGNAL(triggered()), core, SLOT(dvdnavLeft()) );

	dvdnavRightAct = new MyAction(Qt::SHIFT | Qt::Key_Right, this, "dvdnav_right");
	connect( dvdnavRightAct, SIGNAL(triggered()), core, SLOT(dvdnavRight()) );

	dvdnavMenuAct = new MyAction(Qt::SHIFT | Qt::Key_Return, this, "dvdnav_menu");
	connect( dvdnavMenuAct, SIGNAL(triggered()), core, SLOT(dvdnavMenu()) );

	dvdnavSelectAct = new MyAction(Qt::Key_Return, this, "dvdnav_select");
	connect( dvdnavSelectAct, SIGNAL(triggered()), core, SLOT(dvdnavSelect()) );

	dvdnavPrevAct = new MyAction(Qt::SHIFT | Qt::Key_Escape, this, "dvdnav_prev");
	connect( dvdnavPrevAct, SIGNAL(triggered()), core, SLOT(dvdnavPrev()) );

	dvdnavMouseAct = new MyAction( this, "dvdnav_mouse");
	connect( dvdnavMouseAct, SIGNAL(triggered()), core, SLOT(dvdnavMouse()) );
#endif
        //UMPlayer Additions
        youtubeAct = new MyAction(this, "youtube");
        IconSetter::instance()->youtubeAct = youtubeAct;
        connect(youtubeAct, SIGNAL(triggered()), this, SLOT(showYoutube()));                

        shoutcastAct = new MyAction(this, "shoutcast");
        IconSetter::instance()->shoutcastAct = shoutcastAct;        
        connect(shoutcastAct, SIGNAL(triggered()), this, SLOT(showShoutCast()) );

        recordAct = new MyAction(this, "record");        
        connect(recordAct, SIGNAL(triggered()), this, SLOT(recordYoutube())) ;


        //youtubeBox = new YoutubeSearchBox;
        //youtubeBox->setObjectName("search-box");
        //youtubeBox->setMinimumWidth(90);
        //youtubeBox->setMaximumWidth(180);
        //connect(youtubeBox, SIGNAL(search(QString,int)), this, SLOT(searchYT(QString,int)));

        equalizerAct = new MyAction( this, "equalizer" );
        equalizerAct->setCheckable( true );
        connect( equalizerAct, SIGNAL(toggled(bool)),
             this, SLOT(showEqualizer(bool)));

}

void BaseGui::showYoutube()
{
    youTubeDialog->setMode(YTDialog::Button);    
    youTubeDialog->show();
    youTubeDialog->raise();
    youTubeDialog->resize(400, 500);

}

void BaseGui::showShoutCast()
{
    shoutcastDialog->setMode(SCDialog::Button);
    shoutcastDialog->show();
    shoutcastDialog->raise();
    shoutcastDialog->resize(380, 420);
}

void BaseGui::shareUmplayer()
{
    if( sender() == facebookAct )
    {
        QDesktopServices::openUrl(QUrl("http://www.facebook.com/sharer.php?u=http%3A%2F%2Fwww.umplayer.com&t=UMPlayer+|+Universal+Multimedia+Player"));
    }
    else if(sender() == twitterAct)
    {
        QDesktopServices::openUrl(QUrl("http://twitter.com/intent/tweet?text=UMPlayer+|+Universal+Multimedia+Player&url=http%3A%2F%2Fwww.umplayer.com%2F&via=UMPlayer"));
    }
    else if(sender() == mySpaceAct)
    {
        QDesktopServices::openUrl(QUrl("http://www.myspace.com/Modules/PostTo/Pages/?l=3&u=http%3A%2F%2Fwww.umplayer.com%2F&t=UMPlayer+|+Universal+Multimedia+Player&c=%3Cp%3EPowered+by+%3Ca+href%3D%22http%3A%2F%2Fwww.umplayer.com%22%3EUMPlayer%3C%2Fa%3E%3C%2Fp%3E"));
    }
    else if(sender() == gmailAct)
    {
        QDesktopServices::openUrl(QUrl("https://mail.google.com/mail/?view=cm&fs=1&to&su=UMPlayer+|+Universal+Multimedia+Player&body=http://www.umplayer.com/&ui=2&tf=1&shva=1"));
    }
    else if(sender() == hotmailAct)
    {
        QDesktopServices::openUrl(QUrl("http://www.hotmail.msn.com/secure/start?action=compose&to=&subject=UMPlayer+|+Universal+Multimedia+Player&body=http://www.umplayer.com/"));
    }
    else if(sender() == yahooAct)
    {
        QDesktopServices::openUrl(QUrl("http://compose.mail.yahoo.com/?To=&Subject=UMPlayer+|+Universal+Multimedia+Player&body=http%3A%2F%2Fwww.umplayer.com"));
    }
}

void BaseGui::recordYoutube()
{
    QUrl qurl(core->mdat.filename);
    QString id = core->mdat.youtube_id;
    if(core->mdat.filename.toLower().contains("youtube"))
        RecordingDialog::downloadVideoId(id, core->mdat.stream_title, core->mdat.duration);
}

void BaseGui::searchYT(QString searchTerm, int engine)
{
    if(engine == YoutubeSearchBox::Youtube)
    {
        youTubeDialog->setSearchTerm(searchTerm);
        youTubeDialog->show();
        youTubeDialog->raise();
        youTubeDialog->resize(400, 500);        
    }
    else if(engine == YoutubeSearchBox::Shoutcast)
    {
        shoutcastDialog->setSearchTerm(searchTerm);
        shoutcastDialog->show();
        shoutcastDialog->raise();
        shoutcastDialog->resize(400, 500);
    }
}

#if AUTODISABLE_ACTIONS
void BaseGui::setActionsEnabled(bool b) {
	// Menu Play
	playAct->setEnabled(b);
	playOrPauseAct->setEnabled(b);

        recordAct->setEnabled(false);

	pauseAct->setEnabled(b);
	pauseAndStepAct->setEnabled(b);
        playNextAct->setEnabled(b);
        playPrevAct->setEnabled(b);
	stopAct->setEnabled(b);
	frameStepAct->setEnabled(b);
	rewind1Act->setEnabled(b);
	rewind2Act->setEnabled(b);
	rewind3Act->setEnabled(b);
	forward1Act->setEnabled(b);
	forward2Act->setEnabled(b);
	forward3Act->setEnabled(b);
	//repeatAct->setEnabled(b);
	gotoAct->setEnabled(b);

	// Menu Speed
	normalSpeedAct->setEnabled(b);
	halveSpeedAct->setEnabled(b);
	doubleSpeedAct->setEnabled(b);
	decSpeed10Act->setEnabled(b);
	incSpeed10Act->setEnabled(b);
	decSpeed4Act->setEnabled(b);
	incSpeed4Act->setEnabled(b);
	decSpeed1Act->setEnabled(b);
	incSpeed1Act->setEnabled(b);

	// Menu Video
	videoEqualizerAct->setEnabled(b);
        video_equalizer->setEnabled(b);
	screenshotAct->setEnabled(b);
	screenshotsAct->setEnabled(b);
	flipAct->setEnabled(b);
	mirrorAct->setEnabled(b);
	postProcessingAct->setEnabled(b);
	phaseAct->setEnabled(b);
	deblockAct->setEnabled(b);
	deringAct->setEnabled(b);
	addNoiseAct->setEnabled(b);
	addLetterboxAct->setEnabled(b);
	upscaleAct->setEnabled(b);

	// Menu Audio
	audioEqualizerAct->setEnabled(b);
        audio_equalizer->setEnabled(b);
	muteAct->setEnabled(b);
	decVolumeAct->setEnabled(b);
	incVolumeAct->setEnabled(b);
	decAudioDelayAct->setEnabled(b);
	incAudioDelayAct->setEnabled(b);
	audioDelayAct->setEnabled(b);
	extrastereoAct->setEnabled(b);
	karaokeAct->setEnabled(b);
	volnormAct->setEnabled(b);
	loadAudioAct->setEnabled(b);
	//unloadAudioAct->setEnabled(b);

	// Menu Subtitles
	loadSubsAct->setEnabled(b);
	//unloadSubsAct->setEnabled(b);
	decSubDelayAct->setEnabled(b);
	incSubDelayAct->setEnabled(b);
	subDelayAct->setEnabled(b);
	decSubPosAct->setEnabled(b);
	incSubPosAct->setEnabled(b);
	incSubStepAct->setEnabled(b);
	decSubStepAct->setEnabled(b);
	incSubScaleAct->setEnabled(b);
	decSubScaleAct->setEnabled(b);

	// Actions not in menus
#if !USE_MULTIPLE_SHORTCUTS
	decVolume2Act->setEnabled(b);
	incVolume2Act->setEnabled(b);
#endif
	decContrastAct->setEnabled(b);
	incContrastAct->setEnabled(b);
	decBrightnessAct->setEnabled(b);
	incBrightnessAct->setEnabled(b);
	decHueAct->setEnabled(b);
	incHueAct->setEnabled(b);
	decSaturationAct->setEnabled(b);
	incSaturationAct->setEnabled(b);
	decGammaAct->setEnabled(b);
	incGammaAct->setEnabled(b);
	nextVideoAct->setEnabled(b);
	nextAudioAct->setEnabled(b);
	nextSubtitleAct->setEnabled(b);
	nextChapterAct->setEnabled(b);
	prevChapterAct->setEnabled(b);
	doubleSizeAct->setEnabled(b);

	// Moving and zoom
	moveUpAct->setEnabled(b);
	moveDownAct->setEnabled(b);
	moveLeftAct->setEnabled(b);
	moveRightAct->setEnabled(b);
	incZoomAct->setEnabled(b);
	decZoomAct->setEnabled(b);
	resetZoomAct->setEnabled(b);
	autoZoomAct->setEnabled(b);
	autoZoom169Act->setEnabled(b);
	autoZoom235Act->setEnabled(b);

#if DVDNAV_SUPPORT
	dvdnavUpAct->setEnabled(b);
	dvdnavDownAct->setEnabled(b);
	dvdnavLeftAct->setEnabled(b);
	dvdnavRightAct->setEnabled(b);
	dvdnavMenuAct->setEnabled(b);
	dvdnavSelectAct->setEnabled(b);
	dvdnavPrevAct->setEnabled(b);
	dvdnavMouseAct->setEnabled(b);
#endif

        synchro->setEnabled(b);

	// Groups
	denoiseGroup->setActionsEnabled(b);
	sizeGroup->setActionsEnabled(b);
	deinterlaceGroup->setActionsEnabled(b);
	aspectGroup->setActionsEnabled(b);
	rotateGroup->setActionsEnabled(b);
#if USE_ADAPTER
	screenGroup->setActionsEnabled(b);
#endif
	channelsGroup->setActionsEnabled(b);
	stereoGroup->setActionsEnabled(b);

        // YouTube Special

        if(b)
        {
            if(core->mdat.type == TYPE_YOUTUBE)
            {
                recordAct->setEnabled(true);
                rewind1Act->setEnabled(false);
                forward1Act->setEnabled(false);
                qDebug() << "BaseGUI::Recording Enabled";
            }
            else
            {
                recordAct->setEnabled(false);
                qDebug() << "BaseGUI::Recording Disabled";
            }
        }
        else
        {
            recordAct->setEnabled(false);
        }

        if(core->mdat.type == TYPE_YOUTUBE || core->mdat.type == TYPE_STREAM)
        {
            doubleSpeedAct->setEnabled(false);
            halveSpeedAct->setEnabled(false);
        }
        if(qRound(core->mset.speed) != 1  )
            core->normalSpeed();
}

void BaseGui::enableActionsOnPlaying() {
	qDebug("BaseGui::enableActionsOnPlaying");

	setActionsEnabled(true);

	playAct->setEnabled(false);
        playlist->checkNextPrevAvailability();

	// Screenshot option
	bool screenshots_enabled = ( (pref->use_screenshot) &&
                                 (!pref->screenshot_directory.isEmpty()) &&
                                 (QFileInfo(pref->screenshot_directory).isDir()) );

	screenshotAct->setEnabled( screenshots_enabled );
	screenshotsAct->setEnabled( screenshots_enabled );

	// Enable or disable the audio equalizer
	audioEqualizerAct->setEnabled(pref->use_audio_equalizer);

	// Disable audio actions if there's not audio track
	if ((core->mdat.audios.numItems()==0) && (core->mset.external_audio.isEmpty())) {
		audioEqualizerAct->setEnabled(false);
                audio_equalizer->setEnabled(false);
		muteAct->setEnabled(false);
		decVolumeAct->setEnabled(false);
		incVolumeAct->setEnabled(false);
		decAudioDelayAct->setEnabled(false);
		incAudioDelayAct->setEnabled(false);
		audioDelayAct->setEnabled(false);
		extrastereoAct->setEnabled(false);
		karaokeAct->setEnabled(false);
		volnormAct->setEnabled(false);
		channelsGroup->setActionsEnabled(false);
		stereoGroup->setActionsEnabled(false);
	}

	// Disable video actions if it's an audio file
	if (core->mdat.novideo) {
		videoEqualizerAct->setEnabled(false);
                video_equalizer->setEnabled(false);
		screenshotAct->setEnabled(false);
		screenshotsAct->setEnabled(false);
		flipAct->setEnabled(false);
		mirrorAct->setEnabled(false);
		postProcessingAct->setEnabled(false);
		phaseAct->setEnabled(false);
		deblockAct->setEnabled(false);
		deringAct->setEnabled(false);
		addNoiseAct->setEnabled(false);
		addLetterboxAct->setEnabled(false);
		upscaleAct->setEnabled(false);
		doubleSizeAct->setEnabled(false);

		// Moving and zoom
		moveUpAct->setEnabled(false);
		moveDownAct->setEnabled(false);
		moveLeftAct->setEnabled(false);
		moveRightAct->setEnabled(false);
		incZoomAct->setEnabled(false);
		decZoomAct->setEnabled(false);
		resetZoomAct->setEnabled(false);
		autoZoomAct->setEnabled(false);
		autoZoom169Act->setEnabled(false);
		autoZoom235Act->setEnabled(false);

		denoiseGroup->setActionsEnabled(false);
		sizeGroup->setActionsEnabled(false);
		deinterlaceGroup->setActionsEnabled(false);
		aspectGroup->setActionsEnabled(false);
		rotateGroup->setActionsEnabled(false);
#if USE_ADAPTER
		screenGroup->setActionsEnabled(false);
#endif
	}

#if USE_ADAPTER
	screenGroup->setActionsEnabled(pref->vo.startsWith(OVERLAY_VO));
#endif

#if DVDNAV_SUPPORT
	if (!core->mdat.filename.startsWith("dvdnav:")) {
		dvdnavUpAct->setEnabled(false);
		dvdnavDownAct->setEnabled(false);
		dvdnavLeftAct->setEnabled(false);
		dvdnavRightAct->setEnabled(false);
		dvdnavMenuAct->setEnabled(false);
		dvdnavSelectAct->setEnabled(false);
		dvdnavPrevAct->setEnabled(false);
		dvdnavMouseAct->setEnabled(false);
	}
#endif
}

void BaseGui::disableActionsOnStop() {
	qDebug("BaseGui::disableActionsOnStop");

	setActionsEnabled(false);

	playAct->setEnabled(true);
	playOrPauseAct->setEnabled(true);
	stopAct->setEnabled(true);
}

void BaseGui::togglePlayAction(Core::State state) {
	qDebug("BaseGui::togglePlayAction");
	if (state == Core::Playing)
        {
            playAct->setEnabled(false);
            playOrPauseAct->setChecked(true);
       }
	else
        {
            playAct->setEnabled(true);
            playOrPauseAct->setChecked(false);
        }
}
#endif // AUTODISABLE_ACTIONS

void BaseGui::retranslateStrings() {
        QStringList logos;
        logos << "Umplayer-16" << "Umplayer-24" <<"Umplayer-32" << "Umplayer-48" << "Umplayer-256" << "Umplayer-512" ;
        setWindowIcon( Images::icon(logos));

	// ACTIONS

	// Menu File
        openFileAct->change( tr("&Open File...") );
	openPlaylistAct->change( Images::icon("open_playlist"), tr("&Playlist...") );
	openVCDAct->change( Images::icon("vcd"), tr("V&CD") );
	openAudioCDAct->change( Images::icon("cdda"), tr("&Audio CD") );
        openDVDFolderAct->change( Images::icon("dvd_hd"), tr("D&VD from folder...") );
        openDirectoryAct->change( tr("D&irectory...") );
        openDVDAct->change( tr("&DVD from drive") );
        openURLAct->change( tr("&URL...") );
	exitAct->change( Images::icon("close"), tr("C&lose") );

	// TV & Radio submenus
	tvlist->editAct()->setText( tr("&Edit...") );
	radiolist->editAct()->setText( tr("&Edit...") );
	tvlist->jumpAct()->setText( tr("&Jump...") );
	radiolist->jumpAct()->setText( tr("&Jump...") );
	tvlist->nextAct()->setText( tr("Next TV channel") );
	tvlist->previousAct()->setText( tr("Previous TV channel") );
	radiolist->nextAct()->setText( tr("Next radio channel") );
	radiolist->previousAct()->setText( tr("Previous radio channel") );


	// Menu Play
	playAct->change( tr("P&lay") );
	if (qApp->isLeftToRight())
		playAct->setIcon( Images::icon("play") );
	else
		playAct->setIcon( Images::flippedIcon("play") );

	pauseAct->change( Images::icon("pause"), tr("&Pause"));
	stopAct->change( Images::icon("stop"), tr("&Stop") );
	frameStepAct->change( Images::icon("frame_step"), tr("&Frame step") );

	playOrPauseAct->change( tr("Play / Pause") );
	if (qApp->isLeftToRight())
		playOrPauseAct->setIcon( Images::icon("play_pause") );
	else
		playOrPauseAct->setIcon( Images::flippedIcon("play_pause") );

	pauseAndStepAct->change( Images::icon("pause"), tr("Pause / Frame step") );

	setJumpTexts(); // Texts for rewind*Act and forward*Act

	repeatAct->change( Images::icon("repeat"), tr("&Repeat") );
	gotoAct->change( Images::icon("jumpto"), tr("&Jump to...") );

	// Submenu speed
	normalSpeedAct->change( tr("&Normal speed") );
	halveSpeedAct->change( tr("&Halve speed") );
	doubleSpeedAct->change( tr("&Double speed") );
	decSpeed10Act->change( tr("Speed &-10%") );
	incSpeed10Act->change( tr("Speed &+10%") );
	decSpeed4Act->change( tr("Speed -&4%") );
	incSpeed4Act->change( tr("&Speed +4%") );
	decSpeed1Act->change( tr("Speed -&1%") );
	incSpeed1Act->change( tr("S&peed +1%") );

	// Menu Video
	fullscreenAct->change( Images::icon("fullscreen"), tr("&Fullscreen") );
	videoEqualizerAct->change( Images::icon("equalizer"), tr("&Equalizer") );
        screenshotAct->change( tr("&Screenshot") );
	screenshotsAct->change( Images::icon("screenshots"), tr("Start/stop takin&g screenshots") );
	videoPreviewAct->change( Images::icon("video_preview"), tr("Pre&view...") );
	flipAct->change( Images::icon("flip"), tr("Flip i&mage") );
	mirrorAct->change( Images::icon("mirror"), tr("Mirr&or image") );
        motionVectorsAct->change( Images::icon("vector"),
                              tr("Visualize &motion vectors") );

	decZoomAct->change( tr("Zoom &-") );
	incZoomAct->change( tr("Zoom &+") );
	resetZoomAct->change( tr("&Reset") );
	autoZoomAct->change( tr("&Auto zoom") );
	autoZoom169Act->change( tr("Zoom for &16:9") );
	autoZoom235Act->change( tr("Zoom for &2.35:1") );
	moveLeftAct->change( tr("Move &left") );
	moveRightAct->change( tr("Move &right") );
	moveUpAct->change( tr("Move &up") );
	moveDownAct->change( tr("Move &down") );

	// Submenu Filters
	postProcessingAct->change( tr("&Postprocessing") );
	phaseAct->change( tr("&Autodetect phase") );
	deblockAct->change( tr("&Deblock") );
	deringAct->change( tr("De&ring") );
	addNoiseAct->change( tr("Add n&oise") );
	addLetterboxAct->change( Images::icon("letterbox"), tr("Add &black borders") );
	upscaleAct->change( Images::icon("upscaling"), tr("Soft&ware scaling") );

	// Menu Audio
	audioEqualizerAct->change( Images::icon("audio_equalizer"), tr("E&qualizer") );
	QIcon icset( Images::icon("volume") );
	icset.addPixmap( Images::icon("mute"), QIcon::Normal, QIcon::On  );
	muteAct->change( icset, tr("&Mute") );
	decVolumeAct->change( Images::icon("audio_down"), tr("Volume &-") );
	incVolumeAct->change( Images::icon("audio_up"), tr("Volume &+") );
	decAudioDelayAct->change( Images::icon("delay_down"), tr("&Delay -") );
	incAudioDelayAct->change( Images::icon("delay_up"), tr("D&elay +") );
	audioDelayAct->change( Images::icon("audio_delay"), tr("Set dela&y...") );
	loadAudioAct->change( Images::icon("open"), tr("&Load external file...") );
	unloadAudioAct->change( Images::icon("unload"), tr("U&nload") );

	// Submenu Filters
	extrastereoAct->change( tr("&Extrastereo") );
	karaokeAct->change( tr("&Karaoke") );
	volnormAct->change( tr("Volume &normalization") );

	// Menu Subtitles
	loadSubsAct->change( Images::icon("open"), tr("&Load...") );
	unloadSubsAct->change( Images::icon("unload"), tr("U&nload") );
	decSubDelayAct->change( Images::icon("delay_down"), tr("Delay &-") );
	incSubDelayAct->change( Images::icon("delay_up"), tr("Delay &+") );
	subDelayAct->change( Images::icon("sub_delay"), tr("Se&t delay...") );
	decSubPosAct->change( Images::icon("sub_up"), tr("&Up") );
	incSubPosAct->change( Images::icon("sub_down"), tr("&Down") );
	decSubScaleAct->change( Images::icon("dec_sub_scale"), tr("S&ize -") );
	incSubScaleAct->change( Images::icon("inc_sub_scale"), tr("Si&ze +") );
	decSubStepAct->change( Images::icon("dec_sub_step"),
                           tr("&Previous line in subtitles") );
	incSubStepAct->change( Images::icon("inc_sub_step"),
                           tr("N&ext line in subtitles") );
	useAssAct->change( Images::icon("use_ass_lib"), tr("Use SSA/&ASS library") );
	useClosedCaptionAct->change( Images::icon("closed_caption"), tr("Enable &closed caption") );
	useForcedSubsOnlyAct->change( Images::icon("forced_subs"), tr("&Forced subtitles only") );

	subVisibilityAct->change( Images::icon("sub_visibility"), tr("Subtitle &visibility") );

        showFindSubtitlesDialogAct->change( tr("Find subtitles on &OpenSubtitles.org...") );
	openUploadSubtitlesPageAct->change( Images::icon("upload_subs"), tr("Upload su&btitles to OpenSubtitles.org...") );

	// Menu Options
        showRecordingsAct->change(Images::icon("recordings"), tr("&Recordings"));
	showPlaylistAct->change( Images::icon("playlist"), tr("&Playlist") );
        showPropertiesAct->change( tr("View &info and properties...") );
	frameCounterAct->change( Images::icon("frame_counter"),
                             tr("&Show frame counter") );
        showPreferencesAct->change( tr("P&references") );

	// Submenu Logs
	showLogMplayerAct->change( "MPlayer" );
        showLogSmplayerAct->change( "UMPlayer" );

	// Menu Help
        showFAQAct->change( Images::icon("faq"), tr("&Frequently asked questions") );
        homepageAct->change( Images::icon("home"), tr("&Home page") );
        //showCLOptionsAct->change( Images::icon("cl_help"), tr("&Command line options") );
        showTipsAct->change( Images::icon("tips"), tr("&Tips and Tricks") );
        aboutQtAct->change( Images::icon("qt"), tr("About &Qt") );
        aboutThisAct->change( Images::icon(logos), tr("About &UMPlayer") );

        facebookAct->change("Facebook");
        twitterAct->change("Twitter");
        mySpaceAct->change("MySpace");
        gmailAct->change("GMail");
        hotmailAct->change("Hotmail");
        yahooAct->change("Yahoo!");



	// Playlist
	playNextAct->change( tr("&Next") );
	playPrevAct->change( tr("Pre&vious") );
        connect( playlist, SIGNAL(nextAvailable(bool)),
             playNextAct, SLOT(setEnabled(bool)));
        connect( playlist, SIGNAL(prevAvailable(bool)),
             playPrevAct, SLOT(setEnabled(bool)));

	if (qApp->isLeftToRight()) {
		playNextAct->setIcon( Images::icon("next") );
		playPrevAct->setIcon( Images::icon("previous") );
	} else {
		playNextAct->setIcon( Images::flippedIcon("next") );
		playPrevAct->setIcon( Images::flippedIcon("previous") );
	}


	// Actions not in menus or buttons
	// Volume 2
#if !USE_MULTIPLE_SHORTCUTS
	decVolume2Act->change( tr("Dec volume (2)") );
	incVolume2Act->change( tr("Inc volume (2)") );
#endif
	// Exit fullscreen
	exitFullscreenAct->change( tr("Exit fullscreen") );

	nextOSDAct->change( tr("OSD - Next level") );
	decContrastAct->change( tr("Dec contrast") );
	incContrastAct->change( tr("Inc contrast") );
	decBrightnessAct->change( tr("Dec brightness") );
	incBrightnessAct->change( tr("Inc brightness") );
	decHueAct->change( tr("Dec hue") );
	incHueAct->change( tr("Inc hue") );
	decSaturationAct->change( tr("Dec saturation") );
	incSaturationAct->change( tr("Inc saturation") );
	decGammaAct->change( tr("Dec gamma") );
	incGammaAct->change( tr("Inc gamma") );
	nextVideoAct->change( tr("Next video") );
	nextAudioAct->change( tr("Next audio") );
	nextSubtitleAct->change( tr("Next subtitle") );
	nextChapterAct->change( tr("Next chapter") );
	prevChapterAct->change( tr("Previous chapter") );
	doubleSizeAct->change( tr("&Toggle double size") );
	resetVideoEqualizerAct->change( tr("Reset video equalizer") );
	resetAudioEqualizerAct->change( tr("Reset audio equalizer") );
	showContextMenuAct->change( tr("Show context menu") );
	nextAspectAct->change( Images::icon("next_aspect"), tr("Next aspect ratio") );
	nextWheelFunctionAct->change( Images::icon("next_wheel_function"), tr("Next wheel function") );


	// Action groups
	osdNoneAct->change( tr("Subtitles onl&y") );
	osdSeekAct->change( tr("Volume + &Seek") );
	osdTimerAct->change( tr("Volume + Seek + &Timer") );
	osdTotalAct->change( tr("Volume + Seek + Timer + T&otal time") );


	// MENUS
	openMenu->menuAction()->setText( tr("&Open") );
	playMenu->menuAction()->setText( tr("&Play") );
	videoMenu->menuAction()->setText( tr("&Video") );
	audioMenu->menuAction()->setText( tr("&Audio") );
	subtitlesMenu->menuAction()->setText( tr("&Subtitles") );
	browseMenu->menuAction()->setText( tr("&Browse") );
	optionsMenu->menuAction()->setText( tr("Op&tions") );
        toolsMenu->menuAction()->setText(tr("&Tools"));
	helpMenu->menuAction()->setText( tr("&Help") );

	/*
	openMenuAct->setIcon( Images::icon("open_menu") );
	playMenuAct->setIcon( Images::icon("play_menu") );
	videoMenuAct->setIcon( Images::icon("video_menu") );
	audioMenuAct->setIcon( Images::icon("audio_menu") );
	subtitlesMenuAct->setIcon( Images::icon("subtitles_menu") );
	browseMenuAct->setIcon( Images::icon("browse_menu") );
	optionsMenuAct->setIcon( Images::icon("options_menu") );
	helpMenuAct->setIcon( Images::icon("help_menu") );
	*/

	// Menu Open
	recentfiles_menu->menuAction()->setText( tr("&Recent files") );
	recentfiles_menu->menuAction()->setIcon( Images::icon("recents") );
	clearRecentsAct->change( Images::icon("delete"), tr("&Clear") );

	tvlist->menu()->menuAction()->setText( tr("&TV") );
	tvlist->menu()->menuAction()->setIcon( Images::icon("open_tv") );

	radiolist->menu()->menuAction()->setText( tr("Radi&o") );
	radiolist->menu()->menuAction()->setIcon( Images::icon("open_radio") );

	// Menu Play
	speed_menu->menuAction()->setText( tr("Sp&eed") );
	speed_menu->menuAction()->setIcon( Images::icon("speed") );

	// Menu Video
	videotrack_menu->menuAction()->setText( tr("&Track", "video") );
	videotrack_menu->menuAction()->setIcon( Images::icon("video_track") );

	videosize_menu->menuAction()->setText( tr("Si&ze") );
	videosize_menu->menuAction()->setIcon( Images::icon("video_size") );

	panscan_menu->menuAction()->setText( tr("&Pan && scan") );
	panscan_menu->menuAction()->setIcon( Images::icon("panscan") );

	aspect_menu->menuAction()->setText( tr("&Aspect ratio") );
	aspect_menu->menuAction()->setIcon( Images::icon("aspect") );

	deinterlace_menu->menuAction()->setText( tr("&Deinterlace") );
	deinterlace_menu->menuAction()->setIcon( Images::icon("deinterlace") );

	videofilter_menu->menuAction()->setText( tr("F&ilters") );
	videofilter_menu->menuAction()->setIcon( Images::icon("video_filters") );

	rotate_menu->menuAction()->setText( tr("&Rotate") );
	rotate_menu->menuAction()->setIcon( Images::icon("rotate") );

	ontop_menu->menuAction()->setText( tr("S&tay on top") );
	ontop_menu->menuAction()->setIcon( Images::icon("ontop") );

#if USE_ADAPTER
	screen_menu->menuAction()->setText( tr("Scree&n") );
	screen_menu->menuAction()->setIcon( Images::icon("screen") );
#endif

	/*
	denoise_menu->menuAction()->setText( tr("De&noise") );
	denoise_menu->menuAction()->setIcon( Images::icon("denoise") );
	*/

	aspectDetectAct->change( tr("&Auto") );
	aspect11Act->change( "1&:1" );
	aspect32Act->change( "&3:2" );
	aspect43Act->change( "&4:3" );
	aspect54Act->change( "&5:4" );
	aspect149Act->change( "&14:9" );
	aspect1410Act->change( "1&4:10" );
	aspect169Act->change( "16:&9" );
	aspect1610Act->change( "1&6:10" );
	aspect235Act->change( "&2.35:1" );
	aspectNoneAct->change( tr("&Disabled") );

	deinterlaceNoneAct->change( tr("&None") );
	deinterlaceL5Act->change( tr("&Lowpass5") );
	deinterlaceYadif0Act->change( tr("&Yadif (normal)") );
	deinterlaceYadif1Act->change( tr("Y&adif (double framerate)") );
	deinterlaceLBAct->change( tr("Linear &Blend") );
	deinterlaceKernAct->change( tr("&Kerndeint") );

	denoiseNoneAct->change( tr("Denoise o&ff") );
	denoiseNormalAct->change( tr("Denoise nor&mal") );
	denoiseSoftAct->change( tr("Denoise &soft") );

	rotateNoneAct->change( tr("&Off") );
	rotateClockwiseFlipAct->change( tr("&Rotate by 90 degrees clockwise and flip") );
	rotateClockwiseAct->change( tr("Rotate by 90 degrees &clockwise") );
	rotateCounterclockwiseAct->change( tr("Rotate by 90 degrees counterclock&wise") );
	rotateCounterclockwiseFlipAct->change( tr("Rotate by 90 degrees counterclockwise and &flip") );

	onTopAlwaysAct->change( tr("&Always") );
	onTopNeverAct->change( tr("&Never") );
	onTopWhilePlayingAct->change( tr("While &playing") );
	toggleStayOnTopAct->change( tr("Toggle stay on top") );

#if USE_ADAPTER
	screenDefaultAct->change( tr("&Default") );
#endif

	// Menu Audio
	audiotrack_menu->menuAction()->setText( tr("&Track", "audio") );
	audiotrack_menu->menuAction()->setIcon( Images::icon("audio_track") );

	audiofilter_menu->menuAction()->setText( tr("&Filters") );
	audiofilter_menu->menuAction()->setIcon( Images::icon("audio_filters") );

	audiochannels_menu->menuAction()->setText( tr("&Channels") );
	audiochannels_menu->menuAction()->setIcon( Images::icon("audio_channels") );

	stereomode_menu->menuAction()->setText( tr("&Stereo mode") );
	stereomode_menu->menuAction()->setIcon( Images::icon("stereo_mode") );

	/* channelsDefaultAct->change( tr("&Default") ); */
	channelsStereoAct->change( tr("&Stereo") );
	channelsSurroundAct->change( tr("&4.0 Surround") );
	channelsFull51Act->change( tr("&5.1 Surround") );

	stereoAct->change( tr("&Stereo") );
	leftChannelAct->change( tr("&Left channel") );
	rightChannelAct->change( tr("&Right channel") );

	// Menu Subtitle
	subtitlestrack_menu->menuAction()->setText( tr("&Select") );
	subtitlestrack_menu->menuAction()->setIcon( Images::icon("sub") );

	// Menu Browse
	titles_menu->menuAction()->setText( tr("&Title") );
	titles_menu->menuAction()->setIcon( Images::icon("title") );

	chapters_menu->menuAction()->setText( tr("&Chapter") );
	chapters_menu->menuAction()->setIcon( Images::icon("chapter") );

	angles_menu->menuAction()->setText( tr("&Angle") );
	angles_menu->menuAction()->setIcon( Images::icon("angle") );

        //Menu Tools

        youtubeAct->setText( tr("YouTube"));
        shoutcastAct->setText( tr("SHOUTcast"));

#if PROGRAM_SWITCH
	programtrack_menu->menuAction()->setText( tr("P&rogram", "program") );
	programtrack_menu->menuAction()->setIcon( Images::icon("program_track") );
#endif


#if DVDNAV_SUPPORT
	dvdnavUpAct->change(Images::icon("dvdnav_up"), tr("DVD menu, move up"));
	dvdnavDownAct->change(Images::icon("dvdnav_down"), tr("DVD menu, move down"));
	dvdnavLeftAct->change(Images::icon("dvdnav_left"), tr("DVD menu, move left"));
	dvdnavRightAct->change(Images::icon("dvdnav_right"), tr("DVD menu, move right"));
	dvdnavMenuAct->change(Images::icon("dvdnav_menu"), tr("DVD &menu"));
	dvdnavSelectAct->change(Images::icon("dvdnav_select"), tr("DVD menu, select option"));
	dvdnavPrevAct->change(Images::icon("dvdnav_prev"), tr("DVD &previous menu"));
	dvdnavMouseAct->change(Images::icon("dvdnav_mouse"), tr("DVD menu, mouse click"));
#endif

	// Menu Options
	osd_menu->menuAction()->setText( tr("&OSD") );
	osd_menu->menuAction()->setIcon( Images::icon("osd") );

	logs_menu->menuAction()->setText( tr("&View logs") );
	logs_menu->menuAction()->setIcon( Images::icon("logs") );


	// To be sure that the "<empty>" string is translated
	initializeMenus();

	// Other things
        mplayer_log_window->setWindowTitle( tr("UMPlayer - mplayer log") );
        umplayer_log_window->setWindowTitle( tr("UMPlayer - um player log") );

	updateRecents();
	updateWidgets();

	// Update actions view in preferences
	// It has to be done, here. The actions are translated after the
	// preferences dialog.
	if (pref_dialog) pref_dialog->mod_input()->actions_editor->updateView();
}

void BaseGui::setJumpTexts() {
	rewind1Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking1)) );
	rewind2Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking2)) );
	rewind3Act->change( tr("-%1").arg(Helper::timeForJumps(pref->seeking3)) );

	forward1Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking1)) );
	forward2Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking2)) );
	forward3Act->change( tr("+%1").arg(Helper::timeForJumps(pref->seeking3)) );

	if (qApp->isLeftToRight()) {
		rewind1Act->setIcon( Images::icon("rewind10s") );
		rewind2Act->setIcon( Images::icon("rewind1m") );
		rewind3Act->setIcon( Images::icon("rewind10m") );

		forward1Act->setIcon( Images::icon("forward10s") );
		forward2Act->setIcon( Images::icon("forward1m") );
		forward3Act->setIcon( Images::icon("forward10m") );
	} else {
		rewind1Act->setIcon( Images::flippedIcon("rewind10s") );
		rewind2Act->setIcon( Images::flippedIcon("rewind1m") );
		rewind3Act->setIcon( Images::flippedIcon("rewind10m") );

		forward1Act->setIcon( Images::flippedIcon("forward10s") );
		forward2Act->setIcon( Images::flippedIcon("forward1m") );
		forward3Act->setIcon( Images::flippedIcon("forward10m") );
	}
}

void BaseGui::setWindowCaption(const QString & title) {
	setWindowTitle(title);
}

void BaseGui::createCore() {

        core = new Core( mplayerwindow, this );

	connect( core, SIGNAL(menusNeedInitialize()),
             this, SLOT(initializeMenus()) );
	connect( core, SIGNAL(widgetsNeedUpdate()),
             this, SLOT(updateWidgets()) );
	connect( core, SIGNAL(videoEqualizerNeedsUpdate()),
             this, SLOT(updateVideoEqualizer()) );

	connect( core, SIGNAL(audioEqualizerNeedsUpdate()),
             this, SLOT(updateAudioEqualizer()) );

	connect( core, SIGNAL(showFrame(int)),
             this, SIGNAL(frameChanged(int)) );

	connect( core, SIGNAL(showTime(double)),
             this, SLOT(gotCurrentTime(double)) );

	connect( core, SIGNAL(needResize(int, int)),
             this, SLOT(resizeWindow(int,int)) );
	connect( core, SIGNAL(showMessage(QString)),
             this, SLOT(displayMessage(QString)) );
	connect( core, SIGNAL(stateChanged(Core::State)),
             this, SLOT(displayState(Core::State)) );
        connect( core, SIGNAL(fontCachingStarted()),
                 this, SLOT(showFontCacheMesage()));
        connect( core, SIGNAL(fontCachingFinished()),
                 this, SLOT(hideFontCacheMesage()));

	connect( core, SIGNAL(stateChanged(Core::State)),
             this, SLOT(checkStayOnTop(Core::State)), Qt::QueuedConnection );

	connect( core, SIGNAL(mediaStartPlay()),
             this, SLOT(enterFullscreenOnPlay()) );
	connect( core, SIGNAL(mediaStoppedByUser()),
             this, SLOT(exitFullscreenOnStop()) );

	connect( core, SIGNAL(mediaLoaded()),
             this, SLOT(enableActionsOnPlaying()) );
#if NOTIFY_AUDIO_CHANGES
	connect( core, SIGNAL(audioTracksChanged()),
             this, SLOT(enableActionsOnPlaying()) );
#endif
	connect( core, SIGNAL(mediaFinished()),
             this, SLOT(disableActionsOnStop()) );
	connect( core, SIGNAL(mediaStoppedByUser()),
             this, SLOT(disableActionsOnStop()) );

	connect( core, SIGNAL(stateChanged(Core::State)),
             this, SLOT(togglePlayAction(Core::State)) );

	connect( core, SIGNAL(mediaStartPlay()),
             this, SLOT(newMediaLoaded()), Qt::QueuedConnection );


	connect( core, SIGNAL(mediaInfoChanged()),
             this, SLOT(updateMediaInfo()) );

	connect( core, SIGNAL(mediaStartPlay()),
             this, SLOT(checkPendingActionsToRun()), Qt::QueuedConnection );
#if REPORT_OLD_MPLAYER
	connect( core, SIGNAL(mediaStartPlay()),
             this, SLOT(checkMplayerVersion()), Qt::QueuedConnection );
#endif
	connect( core, SIGNAL(failedToParseMplayerVersion(QString)),
             this, SLOT(askForMplayerVersion(QString)) );

	connect( core, SIGNAL(mplayerFailed(QProcess::ProcessError)),
             this, SLOT(showErrorFromMplayer(QProcess::ProcessError)) );

	connect( core, SIGNAL(mplayerFinishedWithError(int)),
             this, SLOT(showExitCodeFromMplayer(int)) );

	// Hide mplayer window
	connect( core, SIGNAL(noVideo()),
             this, SLOT(hidePanel()) );

	// Log mplayer output
	connect( core, SIGNAL(aboutToStartPlaying()),
             this, SLOT(clearMplayerLog()) );
	connect( core, SIGNAL(logLineAvailable(QString)),
             this, SLOT(recordMplayerLog(QString)) );

	connect( core, SIGNAL(mediaLoaded()),
             this, SLOT(autosaveMplayerLog()) );
}

void BaseGui::createMplayerWindow() {

        mplayerwindow = new MplayerWindow( panel);
        mplayerwindow->setObjectName("mplayerwindow");

#if USE_COLORKEY
	mplayerwindow->setColorKey( pref->color_key );
#endif
	mplayerwindow->allowVideoMovement( pref->allow_video_movement );
	QVBoxLayout * layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(mplayerwindow);
        panel->setLayout(layout);

	// mplayerwindow
	/*
    connect( mplayerwindow, SIGNAL(rightButtonReleased(QPoint)),
	         this, SLOT(showPopupMenu(QPoint)) );
	*/

	// mplayerwindow mouse events
	connect( mplayerwindow, SIGNAL(doubleClicked()),
             this, SLOT(doubleClickFunction()) );
	connect( mplayerwindow, SIGNAL(leftClicked()),
             this, SLOT(leftClickFunction()) );
	connect( mplayerwindow, SIGNAL(rightClicked()),
             this, SLOT(rightClickFunction()) );
	connect( mplayerwindow, SIGNAL(middleClicked()),
             this, SLOT(middleClickFunction()) );
	connect( mplayerwindow, SIGNAL(xbutton1Clicked()),
             this, SLOT(xbutton1ClickFunction()) );
	connect( mplayerwindow, SIGNAL(xbutton2Clicked()),
             this, SLOT(xbutton2ClickFunction()) );
	connect( mplayerwindow, SIGNAL(mouseMoved(QPoint)),
             this, SLOT(checkMousePos(QPoint)) );
        connect( mplayerwindow, SIGNAL(mouseMovedDiff(QPoint)),
             this, SLOT(moveWindow(QPoint)));
}

void BaseGui::createVideoEqualizer() {
	// Equalizer        
        video_equalizer = new VideoEqualizer(this);
        connect( video_equalizer->contrast, SIGNAL(valueChanged(int)),
             core, SLOT(setContrast(int)) );
        connect( video_equalizer->brightness, SIGNAL(valueChanged(int)),
             core, SLOT(setBrightness(int)) );
        connect( video_equalizer->hue, SIGNAL(valueChanged(int)),
             core, SLOT(setHue(int)) );
        connect( video_equalizer->saturation, SIGNAL(valueChanged(int)),
             core, SLOT(setSaturation(int)) );
        connect( video_equalizer->gamma, SIGNAL(valueChanged(int)),
             core, SLOT(setGamma(int)) );
	connect( video_equalizer, SIGNAL(visibilityChanged()),
             this, SLOT(updateWidgets()) );
        connect( video_equalizer, SIGNAL(requiresRestart()), core, SLOT(restart()));

}

void BaseGui::createAudioEqualizer() {
	// Audio Equalizer        
            audio_equalizer = new AudioEqualizer(this);
            connect( audio_equalizer->eq[0], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq0(int)) );
            connect( audio_equalizer->eq[1], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq1(int)) );
            connect( audio_equalizer->eq[2], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq2(int)) );
            connect( audio_equalizer->eq[3], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq3(int)) );
            connect( audio_equalizer->eq[4], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq4(int)) );
            connect( audio_equalizer->eq[5], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq5(int)) );
            connect( audio_equalizer->eq[6], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq6(int)) );
            connect( audio_equalizer->eq[7], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq7(int)) );
            connect( audio_equalizer->eq[8], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq8(int)) );
            connect( audio_equalizer->eq[9], SIGNAL(valueChanged(int)),
                 core, SLOT(setAudioEq9(int)) );

            connect( audio_equalizer, SIGNAL(applyClicked(AudioEqualizerList)),
                 core, SLOT(setAudioAudioEqualizerRestart(AudioEqualizerList)) );
            connect( audio_equalizer, SIGNAL(visibilityChanged()),
             this, SLOT(updateWidgets()) );
            connect( audio_equalizer, SIGNAL(requiresRestart()), core, SLOT(restart()));
}

void BaseGui::createEqualizer()
{
    equalizer = new Equalizer(this);
    equalizer->setAudioEqualizer(audio_equalizer);
    equalizer->setVideoEqualizer(video_equalizer);
    equalizer->setSynchronization(synchro);
}

void BaseGui::createSynchronization()
{
    synchro = new Synchronization(this);
    synchro->setCore(core);
    connect( synchro, SIGNAL(visibilityChanged()),
         this, SLOT(updateWidgets()) );
}

void BaseGui::createPlaylist() {

#if DOCK_PLAYLIST
	playlist = new Playlist(core, this, 0);
#else
	//playlist = new Playlist(core, this, "playlist");
	playlist = new Playlist(core, 0);
#endif


	/*
	connect( playlist, SIGNAL(playlistEnded()),
             this, SLOT(exitFullscreenOnStop()) );
	*/
	connect( playlist, SIGNAL(playlistEnded()),
             this, SLOT(playlistHasFinished()) );


	/*
	connect( playlist, SIGNAL(visibilityChanged()),
             this, SLOT(playlistVisibilityChanged()) );
	*/

}

void BaseGui::createPanel() {

        panel = new QWidget( this );
	panel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	panel->setMinimumSize( QSize(1,1) );
        panel->setFocusPolicy( Qt::StrongFocus );

	// panel
	panel->setAutoFillBackground(TRUE);
	ColorUtils::setBackgroundColor( panel, QColor(0,0,0) );
}

void BaseGui::createPreferencesDialog() {
	pref_dialog = new PreferencesDialog(this);
	pref_dialog->setModal(false);
	/* pref_dialog->mod_input()->setActionsList( actions_list ); */
	connect( pref_dialog, SIGNAL(applied()),
             this, SLOT(applyNewPreferences()) );
}

void BaseGui::createFilePropertiesDialog() {
	file_dialog = new FilePropertiesDialog(this);
	file_dialog->setModal(false);
	connect( file_dialog, SIGNAL(applied()),
             this, SLOT(applyFileProperties()) );
}


void BaseGui::createMenus() {
	// MENUS
        openMenu = menuBar()->addMenu("Open");
	playMenu = menuBar()->addMenu("Play");
	videoMenu = menuBar()->addMenu("Video");
	audioMenu = menuBar()->addMenu("Audio");
	subtitlesMenu = menuBar()->addMenu("Subtitles");
	browseMenu = menuBar()->addMenu("Browwse");
	optionsMenu = menuBar()->addMenu("Options");
        toolsMenu = menuBar()->addMenu("Tools");
	helpMenu = menuBar()->addMenu("Help");

	// OPEN MENU
	openMenu->addAction(openFileAct);

	recentfiles_menu = new QMenu(this);
	recentfiles_menu->addAction( clearRecentsAct );
	recentfiles_menu->addSeparator();

	openMenu->addMenu( recentfiles_menu );

	openMenu->addAction(openDirectoryAct);
	openMenu->addAction(openPlaylistAct);
	openMenu->addAction(openDVDAct);
	openMenu->addAction(openDVDFolderAct);
	openMenu->addAction(openVCDAct);
	openMenu->addAction(openAudioCDAct);
	openMenu->addAction(openURLAct);
	openMenu->addMenu(tvlist->menu());
	openMenu->addMenu(radiolist->menu());

	openMenu->addSeparator();
	openMenu->addAction(exitAct);

	// PLAY MENU
	playMenu->addAction(playAct);
	playMenu->addAction(pauseAct);
	/* playMenu->addAction(playOrPauseAct); */
	playMenu->addAction(stopAct);
	playMenu->addAction(frameStepAct);
	playMenu->addSeparator();
	playMenu->addAction(rewind1Act);
	playMenu->addAction(forward1Act);
	playMenu->addAction(rewind2Act);
	playMenu->addAction(forward2Act);
	playMenu->addAction(rewind3Act);
	playMenu->addAction(forward3Act);
	playMenu->addSeparator();

	// Speed submenu
	speed_menu = new QMenu(this);
	speed_menu->menuAction()->setObjectName("speed_menu");
	speed_menu->addAction(normalSpeedAct);
	speed_menu->addSeparator();
	speed_menu->addAction(halveSpeedAct);
	speed_menu->addAction(doubleSpeedAct);
	speed_menu->addSeparator();
	speed_menu->addAction(decSpeed10Act);
	speed_menu->addAction(incSpeed10Act);
	speed_menu->addSeparator();
	speed_menu->addAction(decSpeed4Act);
	speed_menu->addAction(incSpeed4Act);
	speed_menu->addSeparator();
	speed_menu->addAction(decSpeed1Act);
	speed_menu->addAction(incSpeed1Act);

	playMenu->addMenu(speed_menu);

	playMenu->addAction(repeatAct);
	playMenu->addSeparator();
	playMenu->addAction(gotoAct);
	playMenu->addSeparator();
	playMenu->addAction(playPrevAct);
	playMenu->addAction(playNextAct);

	// VIDEO MENU
	videotrack_menu = new QMenu(this);
	videotrack_menu->menuAction()->setObjectName("videotrack_menu");

	videoMenu->addMenu(videotrack_menu);

	videoMenu->addAction(fullscreenAct);        

#if USE_ADAPTER
	// Screen submenu
	screen_menu = new QMenu(this);
	screen_menu->menuAction()->setObjectName("screen_menu");
	screen_menu->addActions( screenGroup->actions() );
	videoMenu->addMenu(screen_menu);
#endif

	// Size submenu
	videosize_menu = new QMenu(this);
	videosize_menu->menuAction()->setObjectName("videosize_menu");
	videosize_menu->addActions( sizeGroup->actions() );
	videosize_menu->addSeparator();
	videosize_menu->addAction(doubleSizeAct);
	videoMenu->addMenu(videosize_menu);

	// Panscan submenu
	panscan_menu = new QMenu(this);
	panscan_menu->menuAction()->setObjectName("panscan_menu");
	panscan_menu->addAction(resetZoomAct);
	panscan_menu->addSeparator();
	panscan_menu->addAction(autoZoomAct);
	panscan_menu->addAction(autoZoom169Act);
	panscan_menu->addAction(autoZoom235Act);
	panscan_menu->addSeparator();
	panscan_menu->addAction(decZoomAct);
	panscan_menu->addAction(incZoomAct);
	panscan_menu->addSeparator();
	panscan_menu->addAction(moveLeftAct);
	panscan_menu->addAction(moveRightAct);
	panscan_menu->addAction(moveUpAct);
	panscan_menu->addAction(moveDownAct);

	videoMenu->addMenu(panscan_menu);

	// Aspect submenu
	aspect_menu = new QMenu(this);
	aspect_menu->menuAction()->setObjectName("aspect_menu");
	aspect_menu->addActions( aspectGroup->actions() );

	videoMenu->addMenu(aspect_menu);

	// Deinterlace submenu
	deinterlace_menu = new QMenu(this);
	deinterlace_menu->menuAction()->setObjectName("deinterlace_menu");
	deinterlace_menu->addActions( deinterlaceGroup->actions() );

	videoMenu->addMenu(deinterlace_menu);

	// Video filter submenu
	videofilter_menu = new QMenu(this);
	videofilter_menu->menuAction()->setObjectName("videofilter_menu");
	videofilter_menu->addAction(postProcessingAct);
	videofilter_menu->addAction(phaseAct);
	videofilter_menu->addAction(deblockAct);
	videofilter_menu->addAction(deringAct);
	videofilter_menu->addAction(addNoiseAct);
	videofilter_menu->addAction(addLetterboxAct);
	videofilter_menu->addAction(upscaleAct);
	videofilter_menu->addSeparator();
	videofilter_menu->addActions(denoiseGroup->actions());

	videoMenu->addMenu(videofilter_menu);

	// Denoise submenu
	/*
	denoise_menu = new QMenu(this);
	denoise_menu->addActions(denoiseGroup->actions());
	videoMenu->addMenu(denoise_menu);
	*/

	// Rotate submenu
	rotate_menu = new QMenu(this);
	rotate_menu->menuAction()->setObjectName("rotate_menu");
	rotate_menu->addActions(rotateGroup->actions());

	videoMenu->addMenu(rotate_menu);

	videoMenu->addAction(flipAct);
	videoMenu->addAction(mirrorAct);
	videoMenu->addSeparator();
	videoMenu->addAction(videoEqualizerAct);
	videoMenu->addAction(screenshotAct);
	videoMenu->addAction(screenshotsAct);

	// Ontop submenu
	ontop_menu = new QMenu(this);
	ontop_menu->menuAction()->setObjectName("ontop_menu");
	ontop_menu->addActions(onTopActionGroup->actions());

	videoMenu->addMenu(ontop_menu);

	videoMenu->addSeparator();
	videoMenu->addAction(videoPreviewAct);
	videoMenu->addSeparator();
	videoMenu->addAction(motionVectorsAct);


    // AUDIO MENU

	// Audio track submenu
	audiotrack_menu = new QMenu(this);
	audiotrack_menu->menuAction()->setObjectName("audiotrack_menu");

	audioMenu->addMenu(audiotrack_menu);

	audioMenu->addAction(loadAudioAct);
	audioMenu->addAction(unloadAudioAct);

	// Filter submenu
	audiofilter_menu = new QMenu(this);
	audiofilter_menu->menuAction()->setObjectName("audiofilter_menu");
	audiofilter_menu->addAction(extrastereoAct);
	audiofilter_menu->addAction(karaokeAct);
	audiofilter_menu->addAction(volnormAct);

	audioMenu->addMenu(audiofilter_menu);

	// Audio channels submenu
	audiochannels_menu = new QMenu(this);
	audiochannels_menu->menuAction()->setObjectName("audiochannels_menu");
	audiochannels_menu->addActions( channelsGroup->actions() );

	audioMenu->addMenu(audiochannels_menu);

	// Stereo mode submenu
	stereomode_menu = new QMenu(this);
	stereomode_menu->menuAction()->setObjectName("stereomode_menu");
	stereomode_menu->addActions( stereoGroup->actions() );

	audioMenu->addMenu(stereomode_menu);
	audioMenu->addAction(audioEqualizerAct);
	audioMenu->addSeparator();
	audioMenu->addAction(muteAct);
	audioMenu->addSeparator();
	audioMenu->addAction(decVolumeAct);
	audioMenu->addAction(incVolumeAct);
	audioMenu->addSeparator();
	audioMenu->addAction(decAudioDelayAct);
	audioMenu->addAction(incAudioDelayAct);
	audioMenu->addSeparator();
	audioMenu->addAction(audioDelayAct);


	// SUBTITLES MENU
	// Track submenu
	subtitlestrack_menu = new QMenu(this);
	subtitlestrack_menu->menuAction()->setObjectName("subtitlestrack_menu");

	subtitlesMenu->addMenu(subtitlestrack_menu);

	subtitlesMenu->addAction(loadSubsAct);
	subtitlesMenu->addAction(unloadSubsAct);
	subtitlesMenu->addSeparator();
	subtitlesMenu->addAction(decSubDelayAct);
	subtitlesMenu->addAction(incSubDelayAct);
	subtitlesMenu->addSeparator();
	subtitlesMenu->addAction(subDelayAct);
	subtitlesMenu->addSeparator();
	subtitlesMenu->addAction(decSubPosAct);
	subtitlesMenu->addAction(incSubPosAct);
	subtitlesMenu->addSeparator();
	subtitlesMenu->addAction(decSubScaleAct);
	subtitlesMenu->addAction(incSubScaleAct);
	subtitlesMenu->addSeparator();
	subtitlesMenu->addAction(decSubStepAct);
	subtitlesMenu->addAction(incSubStepAct);
	subtitlesMenu->addSeparator();
	subtitlesMenu->addAction(useClosedCaptionAct);
	subtitlesMenu->addAction(useForcedSubsOnlyAct);
	subtitlesMenu->addSeparator();
	subtitlesMenu->addAction(subVisibilityAct);
	subtitlesMenu->addSeparator();
	subtitlesMenu->addAction(useAssAct);
	subtitlesMenu->addSeparator(); //turbos
	subtitlesMenu->addAction(showFindSubtitlesDialogAct);
	subtitlesMenu->addAction(openUploadSubtitlesPageAct); //turbos

	// BROWSE MENU
	// Titles submenu
	titles_menu = new QMenu(this);
	titles_menu->menuAction()->setObjectName("titles_menu");

	browseMenu->addMenu(titles_menu);

	// Chapters submenu
	chapters_menu = new QMenu(this);
	chapters_menu->menuAction()->setObjectName("chapters_menu");

	browseMenu->addMenu(chapters_menu);

	// Angles submenu
	angles_menu = new QMenu(this);
	angles_menu->menuAction()->setObjectName("angles_menu");

	browseMenu->addMenu(angles_menu);

#if DVDNAV_SUPPORT
	browseMenu->addSeparator();
	browseMenu->addAction(dvdnavMenuAct);
	browseMenu->addAction(dvdnavPrevAct);
#endif

#if PROGRAM_SWITCH
	programtrack_menu = new QMenu(this);
	programtrack_menu->menuAction()->setObjectName("programtrack_menu");

	browseMenu->addSeparator();
	browseMenu->addMenu(programtrack_menu);
#endif


	// OPTIONS MENU
        optionsMenu->addAction(frameCounterAct);
        sepAct1 = optionsMenu->addSeparator();

	// OSD submenu
	osd_menu = new QMenu(this);
	osd_menu->menuAction()->setObjectName("osd_menu");
	osd_menu->addActions(osdGroup->actions());

	optionsMenu->addMenu(osd_menu);
        sepAct2 = optionsMenu->addSeparator();

        skinMenu = new QMenu(tr("&Skins"), this);
        skinMenu->setIcon(Images::icon("skin"));

        MyActionGroup* skinGroup = new MyActionGroup(this);

        //Add skins
        MyActionGroupItem* noSkin = new MyActionGroupItem(this, skinGroup, "Do Not use skins", "");
        skinMenu->addAction(noSkin);
        if(pref->skin.isEmpty())
            noSkin->setChecked(true);

        // User
        QDir skin_dir = Paths::configPath() + "/themes";
        QStringList skins = skin_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int n=0; n < skins.count(); n++) {
                skinMenu->addAction(new MyActionGroupItem(this, skinGroup, skins[n], skins[n].toAscii().constData()  ));
                if(pref->skin == skins[n])
                    skinMenu->actions().last()->setChecked(true);
        }
        // Global
        skin_dir = Paths::themesPath();
        QStringList skins2 = skin_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int n=0; n < skins2.count(); n++) {
                if (!skins.contains(skins2[n] ) ) {
                        skinMenu->addAction(new MyActionGroupItem(this, skinGroup, skins2[n], skins2[n].toAscii().constData()  ));
                        if(pref->skin == skins2[n])
                            skinMenu->actions().last()->setChecked(true);
                }
        }
        skinMenu->addSeparator();
        QAction* getMoreAct = skinMenu->addAction("Get more Skins");
        getMoreAct->setObjectName("get_more");
        optionsMenu->insertMenu(sepAct2, skinMenu);
        connect(skinMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeSkin(QAction*)));


	// Logs submenu
	logs_menu = new QMenu(this);
	logs_menu->addAction(showLogMplayerAct);
	logs_menu->addAction(showLogSmplayerAct);

	optionsMenu->addMenu(logs_menu);
        optionsMenu->addSeparator();

	optionsMenu->addAction(showPreferencesAct);

	/*
	Favorites * fav = new Favorites(Paths::configPath() + "/test.fav", this);
	connect(fav, SIGNAL(activated(QString)), this, SLOT(open(QString)));
	optionsMenu->addMenu( fav->menu() )->setText("Favorites");
	*/

        //TOOLS MENU

        toolsMenu->addAction(showPlaylistAct);
        //toolsMenu->addAction(showRecordingsAct);
        //toolsMenu->addAction(shoutcastAct);
        //toolsMenu->addAction(youtubeAct);
        toolsMenu->addSeparator();
        toolsMenu->addAction(showPropertiesAct);

	// HELP MENU

        shareMenu = new QMenu(tr("&Share UMPlayer"), this);
        shareMenu->setIcon(Images::icon("share"));
        shareMenu->addAction(facebookAct);
        shareMenu->addAction(twitterAct);
        shareMenu->addAction(mySpaceAct);
        shareMenu->addAction(gmailAct);
        shareMenu->addAction(hotmailAct);
        shareMenu->addAction(yahooAct);

        //helpMenu->addAction(homepageAct);
	//helpMenu->addAction(showTipsAct);
	//helpMenu->addAction(showFAQAct);
	//helpMenu->addSeparator();
        //helpMenu->addMenu(shareMenu);
        //helpMenu->addSeparator();
        helpMenu->addAction(aboutQtAct);
	helpMenu->addAction(aboutThisAct);



	// POPUP MENU
	if (!popup)
		popup = new QMenu(this);
	else
		popup->clear();

	popup->addMenu( openMenu );
	popup->addMenu( playMenu );
	popup->addMenu( videoMenu );
	popup->addMenu( audioMenu );
	popup->addMenu( subtitlesMenu );
	popup->addMenu( browseMenu );
	popup->addMenu( optionsMenu );
        popup->addMenu( toolsMenu);

	// let's show something, even a <empty> entry
	initializeMenus();
}

/*
void BaseGui::closeEvent( QCloseEvent * e )  {
	qDebug("BaseGui::closeEvent");

	qDebug("mplayer_log_window: %d x %d", mplayer_log_window->width(), mplayer_log_window->height() );
        qDebug("umplayer_log_window: %d x %d", umplayer_log_window->width(), umplayer_log_window->height() );

	mplayer_log_window->close();
        umplayer_log_window->close();
	playlist->close();
	equalizer->close();

	core->stop();
	e->accept();
}
*/


void BaseGui::closeWindow(bool quit) {
	qDebug("BaseGui::closeWindow");

	if (core->state() != Core::Stopped) {
		core->stop();
	}

	//qApp->quit();
        if( quit)
            emit quitSolicited();
        else
            emit skinChanged();
}
void BaseGui::changeSkin(QAction* action)
{
    QString skin = action->objectName();
    if(skin == "get_more" && action->text() != skin)
    {
        QDesktopServices::openUrl(QUrl("http://www.umplayer.com/skins/"));
    }
    else
    {
        pref->skin = skin;
        pref->save();        
        closeWindow(false);
    }
}

void BaseGui::showPlaylist() {
	showPlaylist( !playlist->isVisible() );
}

void BaseGui::showRecordings(){
    RecordingDialog::getInstance()->show();
}

void BaseGui::showPlaylist(bool b) {
	if ( !b ) {
		playlist->hide();
	} else {
		exitFullscreenIfNeeded();
		playlist->show();
	}
	//updateWidgets();
}

void BaseGui::showVideoEqualizer() {
        showVideoEqualizer( true );
}

void BaseGui::showVideoEqualizer(bool b) {
	if (!b) {
		video_equalizer->hide();
	} else {
		// Exit fullscreen, otherwise dialog is not visible
		exitFullscreenIfNeeded();
                equalizer->showVideo();
	}
	updateWidgets();
}

void BaseGui::showAudioEqualizer() {
        showAudioEqualizer( true);
}

void BaseGui::showAudioEqualizer(bool b) {
	if (!b) {
		audio_equalizer->hide();
	} else {
		// Exit fullscreen, otherwise dialog is not visible
		exitFullscreenIfNeeded();
                equalizer->showAudio();
	}
	updateWidgets();
}

void BaseGui::showEqualizer()
{
    showEqualizer( !equalizer->isVisible() );
}

void BaseGui::showEqualizer(bool b)
{
    if (!b) {
            equalizer->hide();
    } else {
            // Exit fullscreen, otherwise dialog is not visible
            exitFullscreenIfNeeded();
            equalizer->showAudio();
    }
    updateWidgets();
}

void BaseGui::showPreferencesDialog() {
	qDebug("BaseGui::showPreferencesDialog");

	exitFullscreenIfNeeded();

	if (!pref_dialog) {
		createPreferencesDialog();
	}

	pref_dialog->setData(pref);

	pref_dialog->mod_input()->actions_editor->clear();
	pref_dialog->mod_input()->actions_editor->addActions(this);
#if !DOCK_PLAYLIST
	pref_dialog->mod_input()->actions_editor->addActions(playlist);
#endif
	pref_dialog->show();
}

// The user has pressed OK in preferences dialog
void BaseGui::applyNewPreferences() {
	qDebug("BaseGui::applyNewPreferences");

	bool need_update_language = false;

	pref_dialog->getData(pref);

	if (!pref->default_font.isEmpty()) {
		QFont f;
		f.fromString( pref->default_font );
		qApp->setFont(f);
	}

#ifndef NO_USE_INI_FILES
	PrefGeneral *_general = pref_dialog->mod_general();
	if (_general->fileSettingsMethodChanged()) {
		core->changeFileSettingsMethod(pref->file_settings_method);
	}
#endif

	PrefInterface *_interface = pref_dialog->mod_interface();
	if (_interface->recentsChanged()) {
		updateRecents();
	}
	if (_interface->languageChanged()) need_update_language = true;

        if (_interface->skinChanged()) {
		need_update_language = true;                
                closeWindow(false);
	}

	if (!pref->use_single_instance && server->isListening()) {
		server->close();
		qDebug("BaseGui::applyNewPreferences: server closed");
	}
	else
	{
		bool server_requires_restart = _interface->serverPortChanged();
		if (pref->use_single_instance && !server->isListening())
			server_requires_restart=true;

		if (server_requires_restart) {
			server->close();
			int port = 0;
			if (!pref->use_autoport) port = pref->connection_port;
			if (server->listen(port)) {
				pref->autoport = server->serverPort();
				qDebug("BaseGui::applyNewPreferences: server running on port %d", pref->autoport);
			} else {
				qWarning("BaseGui::applyNewPreferences: server couldn't be started");
			}
		}
	}

	PrefAdvanced *advanced = pref_dialog->mod_advanced();
#if REPAINT_BACKGROUND_OPTION
	if (advanced->repaintVideoBackgroundChanged()) {
		mplayerwindow->videoLayer()->setRepaintBackground(pref->repaint_video_background);
	}
#endif
#if USE_COLORKEY
	if (advanced->colorkeyChanged()) {
		mplayerwindow->setColorKey( pref->color_key );
	}
#endif
	if (advanced->monitorAspectChanged()) {
		mplayerwindow->setMonitorAspect( pref->monitor_aspect_double() );
	}

	if (need_update_language) {
		translator->load(pref->language);
	}

	setJumpTexts(); // Update texts in menus
	updateWidgets(); // Update the screenshot action

    // Restart the video if needed
    if (pref_dialog->requiresRestart())
		core->restart();

	// Update actions
	pref_dialog->mod_input()->actions_editor->applyChanges();
	saveActions();

#ifndef NO_USE_INI_FILES
    pref->save();
#endif
}


void BaseGui::showFilePropertiesDialog() {
	qDebug("BaseGui::showFilePropertiesDialog");

	exitFullscreenIfNeeded();

	if (!file_dialog) {
		createFilePropertiesDialog();
	}

	setDataToFileProperties();

	file_dialog->show();
}

void BaseGui::setDataToFileProperties() {
	// Save a copy of the original values
	if (core->mset.original_demuxer.isEmpty())
		core->mset.original_demuxer = core->mdat.demuxer;

	if (core->mset.original_video_codec.isEmpty())
		core->mset.original_video_codec = core->mdat.video_codec;

	if (core->mset.original_audio_codec.isEmpty())
		core->mset.original_audio_codec = core->mdat.audio_codec;

	QString demuxer = core->mset.forced_demuxer;
	if (demuxer.isEmpty()) demuxer = core->mdat.demuxer;

	QString ac = core->mset.forced_audio_codec;
	if (ac.isEmpty()) ac = core->mdat.audio_codec;

	QString vc = core->mset.forced_video_codec;
	if (vc.isEmpty()) vc = core->mdat.video_codec;

	file_dialog->setDemuxer(demuxer, core->mset.original_demuxer);
	file_dialog->setAudioCodec(ac, core->mset.original_audio_codec);
	file_dialog->setVideoCodec(vc, core->mset.original_video_codec);

	file_dialog->setMplayerAdditionalArguments( core->mset.mplayer_additional_options );
	file_dialog->setMplayerAdditionalVideoFilters( core->mset.mplayer_additional_video_filters );
	file_dialog->setMplayerAdditionalAudioFilters( core->mset.mplayer_additional_audio_filters );

	file_dialog->setMediaData( core->mdat );
}

void BaseGui::applyFileProperties() {
	qDebug("BaseGui::applyFileProperties");

	bool need_restart = false;
	bool demuxer_changed = false;

#undef TEST_AND_SET
#define TEST_AND_SET( Pref, Dialog ) \
	if ( Pref != Dialog ) { Pref = Dialog; need_restart = TRUE; }

	QString prev_demuxer = core->mset.forced_demuxer;

	QString demuxer = file_dialog->demuxer();
	if (demuxer == core->mset.original_demuxer) demuxer="";
	TEST_AND_SET(core->mset.forced_demuxer, demuxer);

	if (prev_demuxer != core->mset.forced_demuxer) {
		// Demuxer changed
		demuxer_changed = true;
		core->mset.current_audio_id = MediaSettings::NoneSelected;
		core->mset.current_sub_id = MediaSettings::NoneSelected;
	}

	QString ac = file_dialog->audioCodec();
	if (ac == core->mset.original_audio_codec) ac="";
	TEST_AND_SET(core->mset.forced_audio_codec, ac);

	QString vc = file_dialog->videoCodec();
	if (vc == core->mset.original_video_codec) vc="";
	TEST_AND_SET(core->mset.forced_video_codec, vc);

	TEST_AND_SET(core->mset.mplayer_additional_options, file_dialog->mplayerAdditionalArguments());
	TEST_AND_SET(core->mset.mplayer_additional_video_filters, file_dialog->mplayerAdditionalVideoFilters());
	TEST_AND_SET(core->mset.mplayer_additional_audio_filters, file_dialog->mplayerAdditionalAudioFilters());

	// Restart the video to apply
	if (need_restart) {
		if (demuxer_changed) {
			core->reload();
		} else {
			core->restart();
		}
	}
}


void BaseGui::updateMediaInfo() {
    qDebug("BaseGui::updateMediaInfo");

	if (file_dialog) {
		if (file_dialog->isVisible()) setDataToFileProperties();
	}

        setWindowCaption( core->mdat.displayName() + " - UMPlayer" );
}

void BaseGui::newMediaLoaded() {
    qDebug("BaseGui::newMediaLoaded");

        if(core->mdat.type != TYPE_YOUTUBE)
            pref->history_recents->addItem( core->mdat.filename );
	updateRecents();

	// If a VCD, Audio CD or DVD, add items to playlist
	bool is_disc = ( (core->mdat.type == TYPE_VCD) || (core->mdat.type == TYPE_DVD) || (core->mdat.type == TYPE_AUDIO_CD) );
#if DVDNAV_SUPPORT
	// Don't add the list of titles if using dvdnav
	if ((core->mdat.type == TYPE_DVD) && (core->mdat.filename.startsWith("dvdnav:"))) is_disc = false;
#endif
	if (pref->auto_add_to_playlist && is_disc)
	{
		int first_title = 1;
		if (core->mdat.type == TYPE_VCD) first_title = pref->vcd_initial_title;

		QString type = "dvd"; // FIXME: support dvdnav
		if (core->mdat.type == TYPE_VCD) type="vcd";
		else
		if (core->mdat.type == TYPE_AUDIO_CD) type="cdda";

		if (core->mset.current_title_id == first_title) {
			playlist->clear();
			QStringList l;
			QString s;
			QString folder;
			if (core->mdat.type == TYPE_DVD) {
				DiscData disc_data = DiscName::split(core->mdat.filename);
				folder = disc_data.device;
			}
			for (int n=0; n < core->mdat.titles.numItems(); n++) {
				s = type + "://" + QString::number(core->mdat.titles.itemAt(n).ID());
				if ( !folder.isEmpty() ) {
					s += "/" + folder; // FIXME: dvd names are not created as they should
				}
				l.append(s);
			}
			playlist->addFiles(l);
			//playlist->setModified(false); // Not a real playlist
		}
	} /*else {
		playlist->clear();
		playlist->addCurrentFile();
	}*/

	if ((core->mdat.type == TYPE_FILE) && (pref->auto_add_to_playlist) && (pref->add_to_playlist_consecutive_files)) {
		// Look for consecutive files
		QStringList files_to_add = Helper::searchForConsecutiveFiles(core->mdat.filename);
		if (!files_to_add.empty()) playlist->addFiles(files_to_add);
	}
        if((int)(core->mdat.duration) == 0)
        {
            rewind1Act->setEnabled(false);
            forward1Act->setEnabled(false);
        }

}


void BaseGui::clearMplayerLog() {
	mplayer_log.clear();
	if (mplayer_log_window->isVisible()) mplayer_log_window->clear();
}

void BaseGui::recordMplayerLog(QString line) {
	if (pref->log_mplayer) {
            if ( (line.indexOf("A:")==-1) && (line.indexOf("V:")==-1) ) {
			line.append("\n");
			mplayer_log.append(line);
			if (mplayer_log_window->isVisible()) mplayer_log_window->appendText(line);
		}
	}
}

void BaseGui::recordUmplayerLog(QString line) {
        if (pref->log_umplayer) {
		line.append("\n");
                umplayer_log.append(line);
                if (umplayer_log_window->isVisible()) umplayer_log_window->appendText(line);
	}
}

/*!
	Save the mplayer log to a file, so it can be used by external
	applications.
*/
void BaseGui::autosaveMplayerLog() {
	qDebug("BaseGui::autosaveMplayerLog");

	if (pref->autosave_mplayer_log) {
		if (!pref->mplayer_log_saveto.isEmpty()) {
			QFile file( pref->mplayer_log_saveto );
			if ( file.open( QIODevice::WriteOnly ) ) {
				QTextStream strm( &file );
				strm << mplayer_log;
				file.close();
			}
		}
	}
}

void BaseGui::showMplayerLog() {
    qDebug("BaseGui::showMplayerLog");

	exitFullscreenIfNeeded();

    mplayer_log_window->setText( mplayer_log );
	mplayer_log_window->show();
}

void BaseGui::showLog() {
    qDebug("BaseGui::showLog");

	exitFullscreenIfNeeded();

        umplayer_log_window->setText( umplayer_log );
    umplayer_log_window->show();
}


void BaseGui::initializeMenus() {
	qDebug("BaseGui::initializeMenus");

#define EMPTY 1

	int n;

	// Subtitles
	subtitleTrackGroup->clear(true);
	QAction * subNoneAct = subtitleTrackGroup->addAction( tr("&None") );
	subNoneAct->setData(MediaSettings::SubNone);
	subNoneAct->setCheckable(true);
	for (n=0; n < core->mdat.subs.numItems(); n++) {
		QAction *a = new QAction(subtitleTrackGroup);
		a->setCheckable(true);
		a->setText(core->mdat.subs.itemAt(n).displayName());
		a->setData(n);
	}
	subtitlestrack_menu->addActions( subtitleTrackGroup->actions() );

	// Audio
	audioTrackGroup->clear(true);
	if (core->mdat.audios.numItems()==0) {
		QAction * a = audioTrackGroup->addAction( tr("<empty>") );
		a->setEnabled(false);
	} else {
		for (n=0; n < core->mdat.audios.numItems(); n++) {
			QAction *a = new QAction(audioTrackGroup);
			a->setCheckable(true);
			a->setText(core->mdat.audios.itemAt(n).displayName());
			a->setData(core->mdat.audios.itemAt(n).ID());
		}
	}
	audiotrack_menu->addActions( audioTrackGroup->actions() );

#if PROGRAM_SWITCH
	// Program
	programTrackGroup->clear(true);
	if (core->mdat.programs.numItems()==0) {
		QAction * a = programTrackGroup->addAction( tr("<empty>") );
		a->setEnabled(false);
	} else {
		for (n=0; n < core->mdat.programs.numItems(); n++) {
			QAction *a = new QAction(programTrackGroup);
			a->setCheckable(true);
			a->setText(core->mdat.programs.itemAt(n).displayName());
			a->setData(core->mdat.programs.itemAt(n).ID());
		}
	}
	programtrack_menu->addActions( programTrackGroup->actions() );
#endif

	// Video
	videoTrackGroup->clear(true);
	if (core->mdat.videos.numItems()==0) {
		QAction * a = videoTrackGroup->addAction( tr("<empty>") );
		a->setEnabled(false);
	} else {
		for (n=0; n < core->mdat.videos.numItems(); n++) {
			QAction *a = new QAction(videoTrackGroup);
			a->setCheckable(true);
			a->setText(core->mdat.videos.itemAt(n).displayName());
			a->setData(core->mdat.videos.itemAt(n).ID());
		}
	}
	videotrack_menu->addActions( videoTrackGroup->actions() );

	// Titles
	titleGroup->clear(true);
	if (core->mdat.titles.numItems()==0) {
		QAction * a = titleGroup->addAction( tr("<empty>") );
		a->setEnabled(false);
	} else {
		for (n=0; n < core->mdat.titles.numItems(); n++) {
			QAction *a = new QAction(titleGroup);
			a->setCheckable(true);
			a->setText(core->mdat.titles.itemAt(n).displayName());
			a->setData(core->mdat.titles.itemAt(n).ID());
		}
	}
	titles_menu->addActions( titleGroup->actions() );

#if GENERIC_CHAPTER_SUPPORT
	chapterGroup->clear(true);
	if (core->mdat.chapters > 0) {
		for (n=0; n < core->mdat.chapters; n++) {
			QAction *a = new QAction(chapterGroup);
			a->setCheckable(true);
			a->setText( QString::number(n+1) );
			a->setData( n + Core::firstChapter() );
		}
	} else {
		QAction * a = chapterGroup->addAction( tr("<empty>") );
		a->setEnabled(false);
	}
	chapters_menu->addActions( chapterGroup->actions() );
#else
	// DVD Chapters
	chapterGroup->clear(true);
	if ( (core->mdat.type == TYPE_DVD) && (core->mset.current_title_id > 0) ) {
		for (n=0; n < core->mdat.titles.item(core->mset.current_title_id).chapters(); n++) {
			QAction *a = new QAction(chapterGroup);
			a->setCheckable(true);
			a->setText( QString::number(n+1) );
			a->setData( n + Core::dvdFirstChapter() );
		}
	} else {
		// *** Matroshka chapters ***
		if (core->mdat.mkv_chapters > 0) {
			for (n=0; n < core->mdat.mkv_chapters; n++) {
				QAction *a = new QAction(chapterGroup);
				a->setCheckable(true);
				a->setText( QString::number(n+1) );
				a->setData( n + Core::firstChapter() );
			}
		} else {
			QAction * a = chapterGroup->addAction( tr("<empty>") );
			a->setEnabled(false);
		}
	}
	chapters_menu->addActions( chapterGroup->actions() );
#endif

	// Angles
	angleGroup->clear(true);
	int n_angles = 0;
	if (core->mset.current_angle_id > 0) {
		int i = core->mdat.titles.find(core->mset.current_angle_id);
		if (i > -1) n_angles = core->mdat.titles.itemAt(i).angles();
	}
	if (n_angles > 0) {
		for (n=1; n <= n_angles; n++) {
			QAction *a = new QAction(angleGroup);
			a->setCheckable(true);
			a->setText( QString::number(n) );
			a->setData( n );
		}
	} else {
		QAction * a = angleGroup->addAction( tr("<empty>") );
		a->setEnabled(false);
	}
	angles_menu->addActions( angleGroup->actions() );
}

void BaseGui::updateRecents() {
	qDebug("BaseGui::updateRecents");

	// Not clear the first 2 items
	while (recentfiles_menu->actions().count() > 2) {
		QAction * a = recentfiles_menu->actions()[2];
		recentfiles_menu->removeAction( a );
		a->deleteLater();
	}

	int current_items = 0;

	if (pref->history_recents->count() > 0) {
		for (int n=0; n < pref->history_recents->count(); n++) {
			QString i = QString::number( n+1 );
			QString fullname = pref->history_recents->item(n);
			QString filename = fullname;
			QFileInfo fi(fullname);
			//if (fi.exists()) filename = fi.fileName(); // Can be slow

			// Let's see if it looks like a file (no dvd://1 or something)
			if (fullname.indexOf(QRegExp("^.*://.*")) == -1) filename = fi.fileName();

			QAction * a = recentfiles_menu->addAction( QString("%1. " + filename ).arg( i.insert( i.size()-1, '&' ), 3, ' ' ));
			a->setStatusTip(fullname);
			a->setData(n);
			connect(a, SIGNAL(triggered()), this, SLOT(openRecent()));
			current_items++;
		}
	} else {
		QAction * a = recentfiles_menu->addAction( tr("<empty>") );
		a->setEnabled(false);
	}

	recentfiles_menu->menuAction()->setVisible( current_items > 0 );
}

void BaseGui::clearRecentsList() {
	// Delete items in menu	
#ifdef Q_OS_WIN32
        clearWindowsMRU();
#endif
        pref->history_recents->clear();
	updateRecents();

}
#ifdef Q_OS_WIN32

void BaseGui::clearWindowsMRU()
{
    if(QSysInfo::windowsVersion() < QSysInfo::WV_WINDOWS7 )
    {
        QDir recentDir = QDir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/recent");
        QStringList recList = recentDir.entryList();

        QStringList recHists = pref->history_recents->toStringList();
        foreach(QString recHist, recHists)
        {
            QFileInfo info(recHist);
            if(info.isFile())
            {
                QString recFileName = info.fileName() + ".lnk";
                if(recList.contains(recFileName))
                {
                    recentDir.remove(recFileName);
                }
            }
        }
    }
    else
    {

        IApplicationDestinations* app_destinations = NULL;

        HRESULT hr = CoCreateInstance(CLSID_ApplicationDestinations, NULL ,CLSCTX_INPROC_SERVER, IID_IApplicationDestinations,
                         reinterpret_cast<void**> (&(app_destinations)));

        if (SUCCEEDED(hr))
        {
            app_destinations->SetAppID(QString(APP_ID).toStdWString().c_str());
            hr = app_destinations->RemoveAllDestinations();
            app_destinations->Release();
        }
    }
}

#endif

void BaseGui::updateWidgets() {
	qDebug("BaseGui::updateWidgets");

	// Subtitles menu
	subtitleTrackGroup->setChecked( core->mset.current_sub_id );

	// Disable the unload subs action if there's no external subtitles
	unloadSubsAct->setEnabled( !core->mset.external_subtitles.isEmpty() );

	// Audio menu
	audioTrackGroup->setChecked( core->mset.current_audio_id );
	channelsGroup->setChecked( core->mset.audio_use_channels );
	stereoGroup->setChecked( core->mset.stereo_mode );
	// Disable the unload audio file action if there's no external audio file
	unloadAudioAct->setEnabled( !core->mset.external_audio.isEmpty() );

#if PROGRAM_SWITCH
	// Program menu
	programTrackGroup->setChecked( core->mset.current_program_id );
#endif

	// Video menu
	videoTrackGroup->setChecked( core->mset.current_video_id );

	// Aspect ratio
	aspectGroup->setChecked( core->mset.aspect_ratio_id );

	// Rotate
	rotateGroup->setChecked( core->mset.rotate );

#if USE_ADAPTER
	screenGroup->setChecked( pref->adapter );
#endif

	// OSD
	osdGroup->setChecked( pref->osd );

	// Titles
	titleGroup->setChecked( core->mset.current_title_id );

	// Chapters
	chapterGroup->setChecked( core->mset.current_chapter_id );

	// Angles
	angleGroup->setChecked( core->mset.current_angle_id );

	// Deinterlace menu
	deinterlaceGroup->setChecked( core->mset.current_deinterlacer );

	// Video size menu
	sizeGroup->setChecked( pref->size_factor );

	// Auto phase
	phaseAct->setChecked( core->mset.phase_filter );

	// Deblock
	deblockAct->setChecked( core->mset.deblock_filter );

	// Dering
	deringAct->setChecked( core->mset.dering_filter );

	// Add noise
	addNoiseAct->setChecked( core->mset.noise_filter );

	// Letterbox
	addLetterboxAct->setChecked( core->mset.add_letterbox );

	// Upscaling
	upscaleAct->setChecked( core->mset.upscaling_filter );


	// Postprocessing
	postProcessingAct->setChecked( core->mset.postprocessing_filter );

	// Denoise submenu
	denoiseGroup->setChecked( core->mset.current_denoiser );

	/*
	// Fullscreen button
	fullscreenbutton->setOn(pref->fullscreen);

	// Mute button
	mutebutton->setOn(core->mset.mute);
	if (core->mset.mute)
		mutebutton->setPixmap( Images::icon("mute_small") );
	else
		mutebutton->setPixmap( Images::icon("volume_small") );

	// Volume slider
	volumeslider->setValue( core->mset.volume );
	*/

	// Mute menu option
	muteAct->setChecked( (pref->global_volume ? pref->mute : core->mset.mute) );

	// Karaoke menu option
	karaokeAct->setChecked( core->mset.karaoke_filter );

	// Extrastereo menu option
	extrastereoAct->setChecked( core->mset.extrastereo_filter );

	// Volnorm menu option
	volnormAct->setChecked( core->mset.volnorm_filter );

	// Repeat menu option
	repeatAct->setChecked( pref->loop );

	// Fullscreen action
	fullscreenAct->setChecked( pref->fullscreen );

	// Time slider
	if (core->state()==Core::Stopped) {
		//FIXME
		//timeslider->setValue( (int) core->mset.current_sec );
	}


        equalizerAct->setChecked( equalizer->isVisible() );

	// Playlist
#if !DOCK_PLAYLIST
	//showPlaylistAct->setChecked( playlist->isVisible() );
#endif

#if DOCK_PLAYLIST
	showPlaylistAct->setChecked( playlist->isVisible() );
#endif

	// Frame counter
	frameCounterAct->setChecked( pref->show_frame_counter );

	// Motion vectors
	motionVectorsAct->setChecked( pref->show_motion_vectors );

	// Stay on top
	onTopActionGroup->setChecked( (int) pref->stay_on_top );

	// Flip
	flipAct->setChecked( core->mset.flip );

	// Mirror
	mirrorAct->setChecked( core->mset.mirror );

	// Use ass lib
	useAssAct->setChecked( pref->use_ass_subtitles );

	// Closed caption and forces subs
	useClosedCaptionAct->setChecked( pref->use_closed_caption_subs );
	useForcedSubsOnlyAct->setChecked( pref->use_forced_subs_only );

	// Subtitle visibility
	subVisibilityAct->setChecked(pref->sub_visibility);

	// Enable or disable subtitle options
	bool e = ((core->mset.current_sub_id != MediaSettings::SubNone) &&
              (core->mset.current_sub_id != MediaSettings::NoneSelected));

	if (pref->use_closed_caption_subs) e = true; // Enable if using closed captions

	decSubDelayAct->setEnabled(e);
	incSubDelayAct->setEnabled(e);
	subDelayAct->setEnabled(e);
	decSubPosAct->setEnabled(e);
	incSubPosAct->setEnabled(e);
	decSubScaleAct->setEnabled(e);
	incSubScaleAct->setEnabled(e);
	decSubStepAct->setEnabled(e);
	incSubStepAct->setEnabled(e);
}

void BaseGui::updateVideoEqualizer() {
	// Equalizer
	video_equalizer->contrast->setValue( core->mset.contrast );
	video_equalizer->brightness->setValue( core->mset.brightness );
	video_equalizer->hue->setValue( core->mset.hue );
	video_equalizer->saturation->setValue( core->mset.saturation );
	video_equalizer->gamma->setValue( core->mset.gamma );
}

void BaseGui::updateAudioEqualizer() {
	// Audio Equalizer
	for (int n = 0; n < 10; n++) {
		audio_equalizer->eq[n]->setValue( core->mset.audio_equalizer[n].toInt() );
	}
}

/*
void BaseGui::playlistVisibilityChanged() {
#if !DOCK_PLAYLIST
	bool visible = playlist->isVisible();

	showPlaylistAct->setChecked( visible );
#endif
}
*/

/*
void BaseGui::openRecent(int item) {
	qDebug("BaseGui::openRecent: %d", item);
	if ((item > -1) && (item < RECENTS_CLEAR)) { // 1000 = Clear item
		open( recents->item(item) );
	}
}
*/

void BaseGui::openRecent() {
	QAction *a = qobject_cast<QAction *> (sender());
	if (a) {
		int item = a->data().toInt();
		qDebug("BaseGui::openRecent: %d", item);
		QString file = pref->history_recents->item(item);

		if (pref->auto_add_to_playlist) {
			if (playlist->maybeSave()) {
				playlist->clear();
				playlist->addFile(file, Playlist::NoGetInfo);

				open( file );
			}
		} else {
			open( file );
		}

	}
}

void BaseGui::open(QString file) {
        qDebug("BaseGui::open: '%s'", file.toUtf8().data());

        QString extension = QFileInfo(file).suffix().toLower();
        QString extensionForQuestion = extension;
        if(extensionForQuestion.contains('?'))
        {
            extensionForQuestion.truncate(extension.indexOf('?'));
        }
        if(extensionForQuestion.contains('|'))
        {
            extensionForQuestion.truncate(extension.indexOf('|'));
        }

#if defined(Q_OS_WIN) && defined(USE_ASSOCIATIONS)
        FileAssocQuestion::instance()->showQuestion(extensionForQuestion);
#endif
        // If file is a playlist, open that playlist

	if ( (extension=="m3u") || (extension=="m3u8") ) {
		playlist->load_m3u(file);
	}
	else
	if (extension=="pls") {
		playlist->load_pls(file);
	}
	else
	if (QFileInfo(file).isDir()) {
		openDirectory(file);
	}        
	else {
		// Let the core to open it, autodetecting the file type
		//if (playlist->maybeSave()) {
		//	playlist->clear();
                //	playlist->addFile(file);

                core->open(file);
		//}
	}

	if (QFile::exists(file)) pref->latest_dir = QFileInfo(file).absolutePath();
}


void BaseGui::openFiles(QStringList files) {
	qDebug("BaseGui::openFiles");
	if (files.empty()) return;

	if (files.count()==1) {
		if (pref->auto_add_to_playlist) {
			if (playlist->maybeSave()) {
				playlist->clear();
				playlist->addFile(files[0], Playlist::NoGetInfo);

				open(files[0]);
			}
		} else {
			open(files[0]);
		}
	} else {
		if (playlist->maybeSave()) {
			playlist->clear();
			playlist->addFiles(files);
			open(files[0]);
		}
	}
}

void BaseGui::openURL() {
	qDebug("BaseGui::openURL");
	exitFullscreenIfNeeded();

	/*
    bool ok;
    QString s = QInputDialog::getText(this,
            tr("UMPlayer - Enter URL"), tr("URL:"), QLineEdit::Normal,
            pref->last_url, &ok );

    if ( ok && !s.isEmpty() ) {

		//playlist->clear();
		//playlistdock->hide();

		openURL(s);
    } else {
        // user entered nothing or pressed Cancel
    }
	*/

	InputURL d(this);

	for (int n=0; n < pref->history_urls->count(); n++) {
		d.setURL( pref->history_urls->url(n), pref->history_urls->isPlaylist(n) );
	}

	if (d.exec() == QDialog::Accepted ) {
		QString url = d.url();
		bool is_playlist = d.isPlaylist();
		if (!url.isEmpty()) {
			pref->history_urls->addUrl(url, is_playlist);
			if (is_playlist) url = url + IS_PLAYLIST_TAG;
			openURL(url);
		}
	}
}

void BaseGui::openURL(QString url, QString title, bool yt, QString id) {
	if (!url.isEmpty()) {
		//pref->history_urls->addUrl(url);

		if (pref->auto_add_to_playlist) {
			if (playlist->maybeSave()) {
                            if(!yt)
                                core->openStream(url, title);
                            else
                                core->openYT(url, title, id);
				playlist->clear();
				playlist->addFile(url, Playlist::NoGetInfo);
			}
		} else {
                    if(!yt)
                        core->openStream(url, title);
                    else
                        core->openYT(url, title, id);
                }
	}
}


void BaseGui::openFile() {
	qDebug("BaseGui::fileOpen");

	exitFullscreenIfNeeded();

	Extensions e;
    QString s = MyFileDialog::getOpenFileName(
                       this, tr("Choose a file"), pref->latest_dir,
                       tr("Multimedia") + e.allPlayable().forFilter()+";;" +
                       tr("Video") + e.video().forFilter()+";;" +
                       tr("Audio") + e.audio().forFilter()+";;" +
                       tr("Playlists") + e.playlist().forFilter()+";;" +
                       tr("All files") +" (*.*)" );

    if ( !s.isEmpty() ) {
		openFile(s);
	}
}

void BaseGui::openFile(QString file) {
	qDebug("BaseGui::openFile: '%s'", file.toUtf8().data());

   if ( !file.isEmpty() ) {

		//playlist->clear();
		//playlistdock->hide();

		// If file is a playlist, open that playlist
		QString extension = QFileInfo(file).suffix().toLower();
		if ( (extension=="m3u") || (extension=="m3u8") ) {
			playlist->load_m3u(file);
		}
		else
		if (extension=="pls") {
			playlist->load_pls(file);
		}
		else
		if (extension=="iso") {
			if (playlist->maybeSave()) {
				core->open(file);
			}
		}
		else {
			if (pref->auto_add_to_playlist) {
				if (playlist->maybeSave()) {
					core->openFile(file);

					playlist->clear();
					playlist->addFile(file, Playlist::NoGetInfo);
				}
			} else {
				core->openFile(file);
			}
                        recordAct->setEnabled(false);
		}
		if (QFile::exists(file)) pref->latest_dir = QFileInfo(file).absolutePath();
	}
}


void BaseGui::configureDiscDevices() {
        QMessageBox::information( this, tr("UMPlayer - Information"),
			tr("The CDROM / DVD drives are not configured yet.\n"
			   "The configuration dialog will be shown now, "
               "so you can do it."), QMessageBox::Ok);

	showPreferencesDialog();
	pref_dialog->showSection( PreferencesDialog::Drives );
}

void BaseGui::openVCD() {
	qDebug("BaseGui::openVCD");

	if ( (pref->dvd_device.isEmpty()) ||
         (pref->cdrom_device.isEmpty()) )
	{
		configureDiscDevices();
	} else {
		if (playlist->maybeSave()) {
			core->openVCD( pref->vcd_initial_title );
		}
	}
}

void BaseGui::openAudioCD() {
	qDebug("BaseGui::openAudioCD");

	if ( (pref->dvd_device.isEmpty()) ||
         (pref->cdrom_device.isEmpty()) )
	{
		configureDiscDevices();
	} else {
		if (playlist->maybeSave()) {
			core->openAudioCD();
		}
	}
}

void BaseGui::openDVD() {
	qDebug("BaseGui::openDVD");

	if ( (pref->dvd_device.isEmpty()) ||
         (pref->cdrom_device.isEmpty()) )
	{
		configureDiscDevices();
	} else {
		if (playlist->maybeSave()) {
#if DVDNAV_SUPPORT
			core->openDVD( DiscName::joinDVD(pref->use_dvdnav ? 0: 1, pref->dvd_device, pref->use_dvdnav) );
#else
			core->openDVD( DiscName::joinDVD(1, pref->dvd_device, false) );
#endif
		}
	}
}

void BaseGui::openDVDFromFolder() {
	qDebug("BaseGui::openDVDFromFolder");

	if (playlist->maybeSave()) {
		InputDVDDirectory *d = new InputDVDDirectory(this);
		d->setFolder( pref->last_dvd_directory );

		if (d->exec() == QDialog::Accepted) {
			qDebug("BaseGui::openDVDFromFolder: accepted");
			openDVDFromFolder( d->folder() );
		}

		delete d;
	}
}

void BaseGui::openDVDFromFolder(QString directory) {
	pref->last_dvd_directory = directory;
#if DVDNAV_SUPPORT
	core->openDVD( DiscName::joinDVD(pref->use_dvdnav ? 0: 1, directory, pref->use_dvdnav) );
#else
	core->openDVD( DiscName::joinDVD(1, directory, false) );
#endif
}

void BaseGui::openDirectory() {
	qDebug("BaseGui::openDirectory");

	QString s = MyFileDialog::getExistingDirectory(
                    this, tr("Choose a directory"),
                    pref->latest_dir );

	if (!s.isEmpty()) {
		openDirectory(s);
	}
}

void BaseGui::openDirectory(QString directory) {
	qDebug("BaseGui::openDirectory: '%s'", directory.toUtf8().data());

	if (Helper::directoryContainsDVD(directory)) {
		core->open(directory);
	}
	else {
		QFileInfo fi(directory);
		if ( (fi.exists()) && (fi.isDir()) ) {
			playlist->clear();
			//playlist->addDirectory(directory);
			playlist->addDirectory( fi.absoluteFilePath() );
			playlist->startPlay();
		} else {
			qDebug("BaseGui::openDirectory: directory is not valid");
		}
	}
}

void BaseGui::loadSub() {
	qDebug("BaseGui::loadSub");

	exitFullscreenIfNeeded();

	Extensions e;
    QString s = MyFileDialog::getOpenFileName(
        this, tr("Choose a file"),
	    pref->latest_dir,
        tr("Subtitles") + e.subtitles().forFilter()+ ";;" +
        tr("All files") +" (*.*)" );

	if (!s.isEmpty()) core->loadSub(s);
}

void BaseGui::setInitialSubtitle(const QString & subtitle_file) {
	qDebug("BaseGui::setInitialSubtitle: '%s'", subtitle_file.toUtf8().constData());

	core->setInitialSubtitle(subtitle_file);
}

void BaseGui::loadAudioFile() {
	qDebug("BaseGui::loadAudioFile");

	exitFullscreenIfNeeded();

	Extensions e;
	QString s = MyFileDialog::getOpenFileName(
        this, tr("Choose a file"),
	    pref->latest_dir,
        tr("Audio") + e.audio().forFilter()+";;" +
        tr("All files") +" (*.*)" );

	if (!s.isEmpty()) core->loadAudioFile(s);
}

void BaseGui::helpFAQ() {	
        QDesktopServices::openUrl(QUrl("http://www.umplayer.com/faq/") );
}

void BaseGui::helpCLOptions() {
	if (clhelp_window == 0) {
		clhelp_window = new LogWindow(this);
	}
        clhelp_window->setWindowTitle( tr("UMPlayer command line options") );
	clhelp_window->setHtml(CLHelp::help(true));
	clhelp_window->show();
}

void BaseGui::helpTips() {
        QDesktopServices::openUrl( QUrl("http://www.umplayer.com/tips/") );
}

void BaseGui::helpHomepage() {
    QDesktopServices::openUrl( QUrl("http://www.umplayer.com/") );

}


void BaseGui::helpAbout() {
	About d(this);
	d.exec();
}

void BaseGui::helpAboutQt() {
	QMessageBox::aboutQt(this, tr("About Qt") );
}

void BaseGui::showGotoDialog() {
	TimeDialog d(this);
	d.setLabel(tr("&Jump to:"));
        d.setWindowTitle(tr("UMPlayer - Seek"));
	d.setMaximumTime( (int) core->mdat.duration);
	d.setTime( (int) core->mset.current_sec);
	if (d.exec() == QDialog::Accepted) {
		core->goToSec( d.time() );
	}
}

void BaseGui::showAudioDelayDialog() {
    equalizer->showAudioDelay();
}

void BaseGui::showSubDelayDialog() {
        equalizer->showSubDelay();
}

void BaseGui::exitFullscreen() {
	if (pref->fullscreen) {
		toggleFullscreen(false);
	}
}

void BaseGui::toggleFullscreen() {
	qDebug("BaseGui::toggleFullscreen");

	toggleFullscreen(!pref->fullscreen);
}

void BaseGui::restore()
{
    toggleFullscreen(false);
}

void BaseGui::toggleFullscreen(bool b) {
	qDebug("BaseGui::toggleFullscreen: %d", b);

	if (b==pref->fullscreen) {
		// Nothing to do
		qDebug("BaseGui::toggleFullscreen: nothing to do, returning");
		return;
	}

	pref->fullscreen = b;

	// If using mplayer window
	if (pref->use_mplayer_window) {
		core->tellmp("vo_fullscreen " + QString::number(b) );
		updateWidgets();
		return;
	}

	if (!panel->isVisible()) return; // mplayer window is not used.

	if (pref->fullscreen) {

		if (pref->restore_pos_after_fullscreen) {
			win_pos = pos();
			win_size = size();
		}

		was_maximized = isMaximized();
		qDebug("BaseGui::toggleFullscreen: was_maximized: %d", was_maximized);

		aboutToEnterFullscreen();

		#ifdef Q_OS_WIN
		// Avoid the video to pause
		if (!pref->pause_when_hidden) hide();
		#endif

		showFullScreen();                

	} else {
		#ifdef Q_OS_WIN
		// Avoid the video to pause
		if (!pref->pause_when_hidden) hide();
		#endif

		showNormal();

		if (was_maximized) showMaximized(); // It has to be called after showNormal()

		aboutToExitFullscreen();

		if (pref->restore_pos_after_fullscreen) {
			move( win_pos );
			resize( win_size );
		}

	}

	updateWidgets();

	if ((pref->add_blackborders_on_fullscreen) &&
        (!core->mset.add_letterbox))
	{
		core->restart();
	}

	setFocus(); // Fixes bug #2493415
}


void BaseGui::aboutToEnterFullscreen() {
    menuBar()->hide();
}

void BaseGui::aboutToExitFullscreen() {
    menuBar()->show();
}

void BaseGui::toggleFrameCounter() {
	toggleFrameCounter( !pref->show_frame_counter );
}

void BaseGui::toggleFrameCounter(bool b) {
    pref->show_frame_counter = b;
    updateWidgets();
}


void BaseGui::leftClickFunction() {
	qDebug("BaseGui::leftClickFunction");

	if (!pref->mouse_left_click_function.isEmpty()) {
		processFunction(pref->mouse_left_click_function);
	}
}

void BaseGui::rightClickFunction() {
	qDebug("BaseGui::rightClickFunction");

	if (!pref->mouse_right_click_function.isEmpty()) {
		processFunction(pref->mouse_right_click_function);
	}
}

void BaseGui::doubleClickFunction() {
	qDebug("BaseGui::doubleClickFunction");

	if (!pref->mouse_double_click_function.isEmpty()) {
		processFunction(pref->mouse_double_click_function);
	}
}

void BaseGui::middleClickFunction() {
	qDebug("BaseGui::middleClickFunction");

	if (!pref->mouse_middle_click_function.isEmpty()) {
		processFunction(pref->mouse_middle_click_function);
	}
}

void BaseGui::xbutton1ClickFunction() {
	qDebug("BaseGui::xbutton1ClickFunction");

	if (!pref->mouse_xbutton1_click_function.isEmpty()) {
		processFunction(pref->mouse_xbutton1_click_function);
	}
}

void BaseGui::xbutton2ClickFunction() {
	qDebug("BaseGui::xbutton2ClickFunction");

	if (!pref->mouse_xbutton2_click_function.isEmpty()) {
		processFunction(pref->mouse_xbutton2_click_function);
	}
}

void BaseGui::processFunction(QString function) {
	qDebug("BaseGui::processFunction: '%s'", function.toUtf8().data());

	QAction * action = ActionsEditor::findAction(this, function);
	if (!action) action = ActionsEditor::findAction(playlist, function);

	if (action) {
		qDebug("BaseGui::processFunction: action found");
		if (action->isCheckable())
			//action->toggle();
			action->trigger();
		else
			action->trigger();
	}
}

void BaseGui::runActions(QString actions) {
	qDebug("BaseGui::runActions");

	actions = actions.simplified(); // Remove white space

	QAction * action;
	QStringList l = actions.split(" ");

	for (int n = 0; n < l.count(); n++) {
		QString a = l[n];
		QString par = "";

		if ( (n+1) < l.count() ) {
			if ( (l[n+1].toLower() == "true") || (l[n+1].toLower() == "false") ) {
				par = l[n+1].toLower();
				n++;
			}
		}

		action = ActionsEditor::findAction(this, a);
		if (!action) action = ActionsEditor::findAction(playlist, a);

		if (action) {
			qDebug("BaseGui::runActions: running action: '%s' (par: '%s')",
                   a.toUtf8().data(), par.toUtf8().data() );

			if (action->isCheckable()) {
				if (par.isEmpty()) {
					//action->toggle();
					action->trigger();
				} else {
					action->setChecked( (par == "true") );
				}
			}
			else {
				action->trigger();
			}
		} else {
			qWarning("BaseGui::runActions: action: '%s' not found",a.toUtf8().data());
		}
	}
}

void BaseGui::checkPendingActionsToRun() {
	qDebug("BaseGui::checkPendingActionsToRun");

	QString actions;
	if (!pending_actions_to_run.isEmpty()) {
		actions = pending_actions_to_run;
		pending_actions_to_run.clear();
		if (!pref->actions_to_run.isEmpty()) {
			actions = pref->actions_to_run +" "+ actions;
		}
	} else {
		actions = pref->actions_to_run;
	}

	if (!actions.isEmpty()) {
		qDebug("BaseGui::checkPendingActionsToRun: actions: '%s'", actions.toUtf8().constData());
		runActions(actions);
	}
}

#if REPORT_OLD_MPLAYER
void BaseGui::checkMplayerVersion() {
	qDebug("BaseGui::checkMplayerVersion");

	// Qt 4.3.5 is crazy, I can't popup a messagebox here, it calls
	// this function once and again when the messagebox is shown

	if ( (pref->mplayer_detected_version > 0) && (!MplayerVersion::isMplayerAtLeast(25158)) ) {
		QTimer::singleShot(1000, this, SLOT(displayWarningAboutOldMplayer()));
	}
}

void BaseGui::displayWarningAboutOldMplayer() {
	qDebug("BaseGui::displayWarningAboutOldMplayer");

	if (!pref->reported_mplayer_is_old) {
		QMessageBox::warning(this, tr("Warning - Using old MPlayer"),
			tr("The version of MPlayer (%1) installed on your system "
               "is obsolete. UMPlayer can't work well with it: some "
               "options won't work, subtitle selection may fail...")
               .arg(MplayerVersion::toString(pref->mplayer_detected_version)) +
            "<br><br>" +
            tr("Please, update your MPlayer.") +
            "<br><br>" +
            tr("(This warning won't be displayed anymore)") );

		pref->reported_mplayer_is_old = true;
	}
	//else
	//statusBar()->showMessage( tr("Using an old MPlayer, please update it"), 10000 );
}
#endif

void BaseGui::dragEnterEvent( QDragEnterEvent *e ) {
	qDebug("BaseGui::dragEnterEvent");

	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}



void BaseGui::dropEvent( QDropEvent *e ) {
	qDebug("BaseGui::dropEvent");

	QStringList files;

	if (e->mimeData()->hasUrls()) {
		QList <QUrl> l = e->mimeData()->urls();
		QString s;
		for (int n=0; n < l.count(); n++) {
			if (l[n].isValid()) {
				qDebug("BaseGui::dropEvent: scheme: '%s'", l[n].scheme().toUtf8().data());
				if (l[n].scheme() == "file")
					s = l[n].toLocalFile();
				else
					s = l[n].toString();
				/*
				qDebug(" * '%s'", l[n].toString().toUtf8().data());
				qDebug(" * '%s'", l[n].toLocalFile().toUtf8().data());
				*/
                                const char* c = s.toUtf8().constData();
                                qDebug() << strlen(c);
                                QString test = QString::fromUtf8(c);
				qDebug("BaseGui::dropEvent: file: '%s'", s.toUtf8().data());
				files.append(s);
			}
		}
	}


	qDebug( "BaseGui::dropEvent: count: %d", files.count());
	if (files.count() > 0) {
		if (files.count() == 1) {
			QFileInfo fi( files[0] );

			Extensions e;
			QRegExp ext_sub(e.subtitles().forRegExp());
			ext_sub.setCaseSensitivity(Qt::CaseInsensitive);
			if (ext_sub.indexIn(fi.suffix()) > -1) {
				qDebug( "BaseGui::dropEvent: loading sub: '%s'", files[0].toUtf8().data());
				core->loadSub( files[0] );
			}
			else
			if (fi.isDir()) {
				openDirectory( files[0] );
			} else {
				//openFile( files[0] );
				if (pref->auto_add_to_playlist) {
					if (playlist->maybeSave()) {
						playlist->clear();
						playlist->addFile(files[0], Playlist::NoGetInfo);

						open( files[0] );
					}
				} else {
					open( files[0] );
				}
			}
		} else {
			// More than one file
			qDebug("BaseGui::dropEvent: adding files to playlist");
			playlist->clear();
			playlist->addFiles(files);
			//openFile( files[0] );
			playlist->startPlay();
		}
	}
}

void BaseGui::showPopupMenu() {
	showPopupMenu(QCursor::pos());
}

void BaseGui::showPopupMenu( QPoint p ) {
	//qDebug("BaseGui::showPopupMenu: %d, %d", p.x(), p.y());
	popup->move( p );
	popup->show();
}

/*
void BaseGui::mouseReleaseEvent( QMouseEvent * e ) {
	qDebug("BaseGui::mouseReleaseEvent");

	if (e->button() == Qt::LeftButton) {
		e->accept();
		emit leftClicked();
	}
	else
	if (e->button() == Qt::MidButton) {
		e->accept();
		emit middleClicked();
	}
	//
	//else
	//if (e->button() == Qt::RightButton) {
	//	showPopupMenu( e->globalPos() );
    //}
	//
	else
		e->ignore();
}

void BaseGui::mouseDoubleClickEvent( QMouseEvent * e ) {
	e->accept();
	emit doubleClicked();
}
*/

void BaseGui::wheelEvent( QWheelEvent * e ) {
	qDebug("BaseGui::wheelEvent: delta: %d", e->delta());
	e->accept();

	if (e->orientation() == Qt::Vertical) {
	    if (e->delta() >= 0)
	        emit wheelUp();
	    else
	        emit wheelDown();
	} else {
		qDebug("BaseGui::wheelEvent: horizontal event received, doing nothing");
	}
}


// Called when a video has started to play
void BaseGui::enterFullscreenOnPlay() {
	if ( (pref->start_in_fullscreen) && (!pref->fullscreen) ) {
		toggleFullscreen(TRUE);
	}
}

// Called when the playlist has stopped
void BaseGui::exitFullscreenOnStop() {
    if (pref->fullscreen) {
		toggleFullscreen(FALSE);
	}
}

void BaseGui::playlistHasFinished() {
	qDebug("BaseGui::playlistHasFinished");
	exitFullscreenOnStop();

	if (pref->close_on_finish) exitAct->trigger();
}

void BaseGui::displayState(Core::State state) {
	qDebug("BaseGui::displayState: %s", core->stateToString().toUtf8().data());
	switch (state) {
		case Core::Playing:	statusBar()->showMessage( tr("Playing %1").arg(core->mdat.filename), 2000); break;
                case Core::Paused:	statusBar()->showMessage( tr("Pause") ); break;
		case Core::Stopped:	statusBar()->showMessage( tr("Stop") , 2000); break;
	}
        if (state == Core::Stopped) setWindowCaption( "UMPlayer" );

#ifdef Q_OS_WIN
	/* Disable screensaver by event */
	just_stopped = false;

	if (state == Core::Stopped) {
		just_stopped = true;
		int time = 1000 * 60; // 1 minute
		QTimer::singleShot( time, this, SLOT(clear_just_stopped()) );
	}
#endif
}

void BaseGui::displayMessage(QString message) {    
	statusBar()->showMessage(message, 2000);        
}

void BaseGui::showFontCacheMesage() {
    //mplayerwindow->showAnimation(true, tr("Please wait while building font cache. This should take less than a few minutes and will only happen once.") );
}

void BaseGui::hideFontCacheMesage() {
    //mplayerwindow->showAnimation(false);
    pref->fontCacheBuilt = true;
}

void BaseGui::gotCurrentTime(double sec) {
	//qDebug( "DefaultGui::displayTime: %f", sec);

	static int last_second = 0;

	if (floor(sec)==last_second) return; // Update only once per second
	last_second = (int) floor(sec);

	QString time = Helper::formatTime( (int) sec ) + " / " +
                           Helper::formatTime( (int) core->mdat.duration );

	//qDebug( " duration: %f, current_sec: %f", core->mdat.duration, core->mset.current_sec);

	emit timeChanged( time );
}

void BaseGui::resizeWindow(int w, int h) {
        qDebug("BaseGui::resizeWindow: %d, %d", w, h);

	// If fullscreen, don't resize!
	if (pref->fullscreen) return;
        if(isMaximized()) return;

	if ( (pref->resize_method==Preferences::Never) && (panel->isVisible()) ) {
		return;
	}

	if (!panel->isVisible()) {
		panel->show();


	}

	if (pref->size_factor != 100) {
		w = w * pref->size_factor / 100;
		h = h * pref->size_factor / 100;
	}

	qDebug("BaseGui::resizeWindow: size to scale: %d, %d", w, h);

	QSize video_size(w,h);

        if (video_size == mplayerwindow->size()) {
		qDebug("BaseGui::resizeWindow: the panel size is already the required size. Doing nothing.");
		return;
	}

        int diff_width = this->width() - mplayerwindow->width();
        int diff_height = this->height() - mplayerwindow->height();

	int new_width = w + diff_width;
	int new_height = h + diff_height;

#if USE_MINIMUMSIZE
	int minimum_width = minimumSizeHint().width();
	if (pref->gui_minimum_width != 0) minimum_width = pref->gui_minimum_width;
	if (new_width < minimum_width) {
		qDebug("BaseGui::resizeWindow: width is too small, setting width to %d", minimum_width);
		new_width = minimum_width;
        }
#endif
        mplayerwindow->showAnimation(false);
	resize(new_width, new_height);        

	qDebug("BaseGui::resizeWindow: done: window size: %d, %d", this->width(), this->height());
	qDebug("BaseGui::resizeWindow: done: panel->size: %d, %d",
           panel->size().width(),
           panel->size().height() );
	qDebug("BaseGui::resizeWindow: done: mplayerwindow->size: %d, %d",
           mplayerwindow->size().width(),
           mplayerwindow->size().height() );

}

void BaseGui::hidePanel() {
	qDebug("BaseGui::hidePanel");

#if ALLOW_TO_HIDE_VIDEO_WINDOW_ON_AUDIO_FILES	
		mplayerwindow->showLogo(true);                
                return;
#endif
	if (panel->isVisible()) {
		// Exit from fullscreen mode
	    if (pref->fullscreen) { toggleFullscreen(false); update(); }

                //resizeWindow( size().width(), 0 );
		int width = size().width();
		if (width > pref->default_size.width()) width = pref->default_size.width();
		resize( width, size().height() - panel->size().height() );
		panel->hide();

	}
}

void BaseGui::resizeEvent(QResizeEvent *r)
{
    if(core->state() == Core::Paused)
    {
        resizeFrameStep->start();
    }
    QMainWindow::resizeEvent(r);
}

void BaseGui::moveEvent(QMoveEvent *m)
{    
    if(m->oldPos() != m->pos())
    {
        if(core->state() == Core::Paused)
        {
            resizeFrameStep->start();
        }
    }
    QMainWindow::moveEvent(m);
}

void BaseGui::displayGotoTime(int t) {
#ifdef SEEKBAR_RESOLUTION
	int jump_time = (int)core->mdat.duration * t / SEEKBAR_RESOLUTION;
#else
	int jump_time = (int)core->mdat.duration * t / 100;
#endif
	QString s = tr("Jump to %1").arg( Helper::formatTime(jump_time) );
	statusBar()->showMessage( s, 1000 );

	if (pref->fullscreen) {
		core->tellmp("osd_show_text \"" + s + "\" 3000 1");
	}
}

void BaseGui::goToPosOnDragging(int t) {
	if (pref->update_while_seeking) {
#if ENABLE_DELAYED_DRAGGING
		#ifdef SEEKBAR_RESOLUTION
		core->goToPosition(t);
		#else
		core->goToPos(t);
		#endif
#else
		if ( ( t % 4 ) == 0 ) {
			qDebug("BaseGui::goToPosOnDragging: %d", t);
			#ifdef SEEKBAR_RESOLUTION
			core->goToPosition(t);
			#else
			core->goToPos(t);
			#endif
		}
#endif
	}
}


void BaseGui::setStayOnTop(bool b) {
	qDebug("BaseGui::setStayOnTop: %d", b);

	if ( (b && (windowFlags() & Qt::WindowStaysOnTopHint)) ||
         (!b && (!(windowFlags() & Qt::WindowStaysOnTopHint))) )
	{
		// identical do nothing
		qDebug("BaseGui::setStayOnTop: nothing to do");
		return;
	}

	ignore_show_hide_events = true;

	bool visible = isVisible();

	QPoint old_pos = pos();

	if (b) {
		setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	}
	else {
		setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
	}

	move(old_pos);

	if (visible) {
		show();
	}

	ignore_show_hide_events = false;
}

void BaseGui::changeStayOnTop(int stay_on_top) {
	switch (stay_on_top) {
		case Preferences::AlwaysOnTop : setStayOnTop(true); break;
		case Preferences::NeverOnTop  : setStayOnTop(false); break;
		case Preferences::WhilePlayingOnTop : setStayOnTop((core->state() == Core::Playing)); break;
	}

	pref->stay_on_top = (Preferences::OnTop) stay_on_top;
	updateWidgets();
}

void BaseGui::checkStayOnTop(Core::State state) {
	qDebug("BaseGui::checkStayOnTop");
    if ((!pref->fullscreen) && (pref->stay_on_top == Preferences::WhilePlayingOnTop)) {
                setStayOnTop((state == Core::Playing));
	}
}

void BaseGui::toggleStayOnTop() {
	if (pref->stay_on_top == Preferences::AlwaysOnTop)
		changeStayOnTop(Preferences::NeverOnTop);
	else
	if (pref->stay_on_top == Preferences::NeverOnTop)
		changeStayOnTop(Preferences::AlwaysOnTop);
}

// Called when a new window (equalizer, preferences..) is opened.
void BaseGui::exitFullscreenIfNeeded() {
	/*
	if (pref->fullscreen) {
		toggleFullscreen(FALSE);
	}
	*/
}

void BaseGui::checkMousePos(QPoint p) {
	//qDebug("BaseGui::checkMousePos: %d, %d", p.x(), p.y());

        if (!pref->fullscreen)
        {
            return;
        }


        /*

	#define MARGIN 70

        int margin = floatingWidgetHeight + pref->floating_control_margin;

	if (p.y() > mplayerwindow->height() - margin) {
		//qDebug("BaseGui::checkMousePos: %d, %d", p.x(), p.y());
		if (!near_bottom) {
			emit cursorNearBottom(p);
			near_bottom = true;
		}
	} else {
		if (near_bottom) {
			emit cursorFarEdges();
			near_bottom = false;
		}
	}

	if (p.y() < margin) {
		//qDebug("BaseGui::checkMousePos: %d, %d", p.x(), p.y());
		if (!near_top) {
			emit cursorNearTop(p);
			near_top = true;
		}
	} else {
		if (near_top) {
			emit cursorFarEdges();
			near_top = false;
		}
        }*/
}

void BaseGui::moveWindow(QPoint diff)
{
    if (pref->fullscreen || isMaximized())
    {
        return;
    }
    move(pos() + diff);
}

void BaseGui::loadActions() {
	qDebug("BaseGui::loadActions");
	ActionsEditor::loadFromConfig(this, settings);
#if !DOCK_PLAYLIST
	ActionsEditor::loadFromConfig(playlist, settings);
#endif

	actions_list = ActionsEditor::actionsNames(this);
#if !DOCK_PLAYLIST
	actions_list += ActionsEditor::actionsNames(playlist);
#endif

	//if (server)
		server->setActionsList( actions_list );
}

void BaseGui::saveActions() {
	qDebug("BaseGui::saveActions");

	ActionsEditor::saveToConfig(this, settings);
#if !DOCK_PLAYLIST
	ActionsEditor::saveToConfig(playlist, settings);
#endif
}


void BaseGui::showEvent( QShowEvent * ) {
	qDebug("BaseGui::showEvent");

	if (ignore_show_hide_events) return;

	//qDebug("BaseGui::showEvent: pref->pause_when_hidden: %d", pref->pause_when_hidden);
	if ((pref->pause_when_hidden) && (core->state() == Core::Paused)) {
		qDebug("BaseGui::showEvent: unpausing");
		core->pause(); // Unpauses
	}
}

void BaseGui::hideEvent( QHideEvent * ) {
	qDebug("BaseGui::hideEvent");

	if (ignore_show_hide_events) return;

	//qDebug("BaseGui::hideEvent: pref->pause_when_hidden: %d", pref->pause_when_hidden);
	if ((pref->pause_when_hidden) && (core->state() == Core::Playing)) {
		qDebug("BaseGui::hideEvent: pausing");
		core->pause();
	}
}

void BaseGui::askForMplayerVersion(QString line) {
	qDebug("BaseGui::askForMplayerVersion: %s", line.toUtf8().data());

	if (pref->mplayer_user_supplied_version <= 0) {
		InputMplayerVersion d(this);
		d.setVersion( pref->mplayer_user_supplied_version );
		d.setVersionFromOutput(line);
		if (d.exec() == QDialog::Accepted) {
			pref->mplayer_user_supplied_version = d.version();
			qDebug("BaseGui::askForMplayerVersion: user supplied version: %d", pref->mplayer_user_supplied_version);
		}
	} else {
		qDebug("BaseGui::askForMplayerVersion: already have a version supplied by user, so no asking");
	}
}

void BaseGui::showExitCodeFromMplayer(int exit_code) {
	qDebug("BaseGui::showExitCodeFromMplayer: %d", exit_code);

	if (!pref->report_mplayer_crashes) {
		qDebug("BaseGui::showExitCodeFromMplayer: not displaying error dialog");
		return;
	}

	if (exit_code != 255 ) {
		ErrorDialog d(this);
		d.setText(tr("MPlayer has finished unexpectedly.") + " " +
	              tr("Exit code: %1").arg(exit_code));
		d.setLog( mplayer_log );
                //d.exec();
	}
}

void BaseGui::showErrorFromMplayer(QProcess::ProcessError e) {
	qDebug("BaseGui::showErrorFromMplayer");

	if (!pref->report_mplayer_crashes) {
		qDebug("showErrorFromMplayer: not displaying error dialog");
		return;
	}

	if ((e == QProcess::FailedToStart) || (e == QProcess::Crashed)) {
		ErrorDialog d(this);
		if (e == QProcess::FailedToStart) {
			d.setText(tr("MPlayer failed to start.") + " " +
                         tr("Please check the MPlayer path in preferences."));
		} else {
			d.setText(tr("MPlayer has crashed.") + " " +
                      tr("See the log for more info."));
		}
		d.setLog( mplayer_log );
                //d.exec();
	}
}


void BaseGui::showFindSubtitlesDialog() {
	qDebug("BaseGui::showFindSubtitlesDialog");

	if (!find_subs_dialog) {
		find_subs_dialog = new FindSubtitlesWindow(this, Qt::Window | Qt::WindowMinMaxButtonsHint);
		find_subs_dialog->setSettings(Global::settings);
		find_subs_dialog->setWindowIcon(windowIcon());
#if DOWNLOAD_SUBS
		connect(find_subs_dialog, SIGNAL(subtitleDownloaded(const QString &)),
                core, SLOT(loadSub(const QString &)));
#endif
	}

	find_subs_dialog->show();
	find_subs_dialog->setMovie(core->mdat.filename);
}

void BaseGui::openUploadSubtitlesPage() {
	//QDesktopServices::openUrl( QUrl("http://ds6.ovh.org/hashsubtitles/upload.php") );
	//QDesktopServices::openUrl( QUrl("http://www.opensubtitles.com/upload") );
	QDesktopServices::openUrl( QUrl("http://www.opensubtitles.org/uploadjava") );
}

void BaseGui::showVideoPreviewDialog() {
	qDebug("BaseGui::showVideoPreviewDialog");

	if (video_preview == 0) {
		video_preview = new VideoPreview( pref->mplayer_bin, this );
		video_preview->setSettings(Global::settings);
	}

	if (!core->mdat.filename.isEmpty()) {
		video_preview->setVideoFile(core->mdat.filename);

		// DVD
		if (core->mdat.type==TYPE_DVD) {
			QString file = core->mdat.filename;
			DiscData disc_data = DiscName::split(file);
			QString dvd_folder = disc_data.device;
			if (dvd_folder.isEmpty()) dvd_folder = pref->dvd_device;
			int dvd_title = disc_data.title;
			file = disc_data.protocol + "://" + QString::number(dvd_title);

			video_preview->setVideoFile(file);
			video_preview->setDVDDevice(dvd_folder);
		} else {
			video_preview->setDVDDevice("");
		}
	}

	video_preview->setMplayerPath(pref->mplayer_bin);

	if ( (video_preview->showConfigDialog(this)) && (video_preview->createThumbnails()) ) {
		video_preview->show();
		video_preview->adjustWindowSize();
	}
}

// Language change stuff
void BaseGui::changeEvent(QEvent *e) {
	if (e->type() == QEvent::LanguageChange) {
		retranslateStrings();
	} else {
		QMainWindow::changeEvent(e);
	}
}

void BaseGui::play_or_pause(bool checked)
{
    playOrPauseAct->setChecked(!checked);
    core->play_or_pause();
}

void BaseGui::playYTUrl(const QMap<int, QString> &qualityMap, QString title, QString id)
{
    QString url;
    switch(pref->playback_quality)
    {
    case YTDialog::FullHD:
        url = qualityMap.value(YTDialog::FullHD, QString());
        if(!url.isNull()) break;
    case YTDialog::HD:
        url = qualityMap.value(YTDialog::HD, QString());
        if(!url.isNull()) break;
    case YTDialog::Normal:
        url = qualityMap.value(YTDialog::Normal, QString());
    }
    raise();
    activateWindow();
    openURL(url, title, true, id);
}

void BaseGui::playSCUrl(QString url, QString title)
{
    raise();
    activateWindow();
    openURL(url, title);
}

#ifdef Q_OS_WIN
/* Disable screensaver by event */
bool BaseGui::winEvent ( MSG * m, long * result ) {
	//qDebug("BaseGui::winEvent");
	if (m->message==WM_SYSCOMMAND) {
		if ((m->wParam & 0xFFF0)==SC_SCREENSAVE || (m->wParam & 0xFFF0)==SC_MONITORPOWER) {
			qDebug("BaseGui::winEvent: received SC_SCREENSAVE or SC_MONITORPOWER");
			qDebug("BaseGui::winEvent: avoid_screensaver: %d", pref->avoid_screensaver);
			qDebug("BaseGui::winEvent: playing: %d", core->state()==Core::Playing);
			qDebug("BaseGui::winEvent: video: %d", !core->mdat.novideo);

			if ((pref->avoid_screensaver) && (core->state()==Core::Playing) && (!core->mdat.novideo)) {
				qDebug("BaseGui::winEvent: not allowing screensaver");
				(*result) = 0;
				return true;
			} else {
				if ((pref->avoid_screensaver) && (just_stopped)) {
					qDebug("BaseGui::winEvent: file just stopped, so not allowing screensaver for a while");
					(*result) = 0;
					return true;
				} else {
					qDebug("BaseGui::winEvent: allowing screensaver");
					return false;
				}
			}
		}
	}
	return false;
}

void BaseGui::clear_just_stopped() {
	qDebug("BaseGui::clear_just_stopped");
	just_stopped = false;
}
#endif

#include "moc_basegui.cpp"
