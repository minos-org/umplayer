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

#include "umplayer.h"
#include "defaultgui.h"
#include "minigui.h"
#include "mpcgui.h"
#include "global.h"
#include "paths.h"
#include "translator.h"
#include "version.h"
#include "constants.h"
#include "myclient.h"
#include "clhelp.h"
#include "qpropertysetter.h"


#include <QDir>
#include <QApplication>

#include <stdio.h>

#ifdef Q_OS_WIN
#if USE_ASSOCIATIONS
#include "extensions.h"
#include "winfileassoc.h"	//required for Uninstall
#endif
#endif
#ifdef Q_OS_MACX
#include "macstyle.h"
#endif


using namespace Global;

UMPlayer::UMPlayer(const QString & config_path, QObject * parent )
	: QObject(parent)
{
	main_window = 0;
        gui_to_use = "MpcGui";

	move_gui = false;
	resize_gui = false;

    Paths::setAppPath( qApp->applicationDirPath() );

#ifndef PORTABLE_APP
	if (config_path.isEmpty()) createConfigDirectory();
#endif
	global_init(config_path);

	// Application translations
	translator->load( pref->language );
	showInfo();
}

UMPlayer::~UMPlayer() {
	if (main_window != 0) delete main_window;
	global_end();
}

BaseGui * UMPlayer::gui() {
            return gui(gui_to_use.toLower());
}

BaseGui * UMPlayer::gui(QString skin) {
    if (main_window == 0) {
            // Changes to app path, so umplayer can find a relative mplayer path
            QDir::setCurrent(Paths::appPath());
            qDebug("UMPlayer::gui: changed working directory to app path");
            qDebug("UMPlayer::gui: current directory: %s", QDir::currentPath().toUtf8().data());
            qApp->setStyleSheet("");

            if (skin == "mpcgui" || skin == "")
            {
#ifdef Q_OS_MACX
                   qApp->setStyle("macintosh");
#endif
                   IconSetter::removeInstance();
                   IconSetter::instance();
                   main_window = new MpcGui(0);

            }
            else
            {
#ifdef Q_OS_MACX
                    qApp->setStyle(new MacStyle());
#endif
                    IconSetter::removeInstance();
                    IconSetter::instance();
                    main_window = new DefaultGui(0);
            }


            if (move_gui) {
                    qDebug("UMPlayer::gui: moving main window to %d %d", gui_position.x(), gui_position.y());
                    main_window->move(gui_position);
            }
            if (resize_gui) {
                    qDebug("UMPlayer::gui: resizing main window to %dx%d", gui_size.width(), gui_size.height());
                    main_window->resize(gui_size);
            }
            connect(main_window, SIGNAL(skinChanged()), this, SLOT(changeSkin()));
    }
    return main_window;

}

void UMPlayer::changeSkin() {
        connect(main_window, SIGNAL(destroyed(QObject*)), this, SLOT(baseGuiDestroyed(QObject*)));
        main_window->deleteLater();
}

void UMPlayer::baseGuiDestroyed(QObject *destroyed) {
    main_window = 0;
    *basegui_instance = 0;
    gui_to_use = pref->skin;
    main_window = gui();
    *basegui_instance = main_window;
    connect(main_window, SIGNAL(quitSolicited()), qApp, SLOT(quit()));
    start();
}

UMPlayer::ExitCode UMPlayer::processArgs(QStringList args) {
	qDebug("UMPlayer::processArgs: arguments: %d", args.count());
	for (int n = 0; n < args.count(); n++) {
		qDebug("UMPlayer::processArgs: %d = %s", n, args[n].toUtf8().data());
	}


    QString action; // Action to be passed to running instance
	int close_at_end = -1; // -1 = not set, 1 = true, 0 false
	int start_in_fullscreen = -1;
	bool show_help = false;

        if (!pref->skin.isEmpty()) gui_to_use = pref->skin;
	bool add_to_playlist = false;

	bool is_playlist = false;

#ifdef Q_OS_WIN
	if (args.contains("-uninstall")){
#if USE_ASSOCIATIONS
		//Called by uninstaller. Will restore old associations.
		WinFileAssoc RegAssoc;
		Extensions exts;
		QStringList regExts;
		RegAssoc.GetRegisteredExtensions(exts.multimedia(), regExts);
		RegAssoc.RestoreFileAssociations(regExts);
		printf("Restored associations\n");
#endif
		return NoError;
	}
#endif

	for (int n = 1; n < args.count(); n++) {
		QString argument = args[n];

		if (argument == "-send-action") {
			if (n+1 < args.count()) {
				n++;
				action = args[n];
			} else {
				printf("Error: expected parameter for -send-action\r\n");
				return ErrorArgument;
			}
		}
		else
		if (argument == "-actions") {
			if (n+1 < args.count()) {
				n++;
				actions_list = args[n];
			} else {
				printf("Error: expected parameter for -actions\r\n");
				return ErrorArgument;
			}
		}
		else
		if (argument == "-sub") {
			if (n+1 < args.count()) {
				n++;
				QString file = args[n];
				if (QFile::exists(file)) {
					subtitle_file = QFileInfo(file).absoluteFilePath();
				} else {
                                        printf("Error: file '%s' doesn't exist\r\n", file.toUtf8().constData());
				}
			} else {
				printf("Error: expected parameter for -sub\r\n");
				return ErrorArgument;
			}
		}
                else
                if( argument == "-play-dir")
                {
                        if (n+1 < args.count()) {
                                n++;
                                QString file = args[n];
                                QFileInfo fileInfo(file);
                                if (fileInfo.exists() && fileInfo.isDir()) {
                                        directory_play = fileInfo.absoluteFilePath();
                                } else {
                                        printf("Error: Either '%s' doesn't exist or it is not directory.\r\n", file.toUtf8().constData());
                                }
                        } else {
                                printf("Error: expected parameter for -play-dir.\r\n");
                                return ErrorArgument;
                        }
                }
		else
		if (argument == "-pos") {
			if (n+2 < args.count()) {
				bool ok_x, ok_y;
				n++;
				gui_position.setX( args[n].toInt(&ok_x) );
				n++;
				gui_position.setY( args[n].toInt(&ok_y) );
				if (ok_x && ok_y) move_gui = true;
			} else {
				printf("Error: expected parameter for -pos\r\n");
				return ErrorArgument;
			}
		}
		else
		if (argument == "-size") {
			if (n+2 < args.count()) {
				bool ok_width, ok_height;
				n++;
				gui_size.setWidth( args[n].toInt(&ok_width) );
				n++;
				gui_size.setHeight( args[n].toInt(&ok_height) );
				if (ok_width && ok_height) resize_gui = true;
			} else {
				printf("Error: expected parameter for -resize\r\n");
				return ErrorArgument;
			}
		}
		else
		if (argument == "-playlist") {
			is_playlist = true;
		}
		else
		if ((argument == "--help") || (argument == "-help") ||
            (argument == "-h") || (argument == "-?") )
		{
			show_help = true;
		}
		else
		if (argument == "-close-at-end") {
			close_at_end = 1;
		}
		else
		if (argument == "-no-close-at-end") {
			close_at_end = 0;
		}
		else
		if (argument == "-fullscreen") {
			start_in_fullscreen = 1;
		}
		else
		if (argument == "-no-fullscreen") {
			start_in_fullscreen = 0;
		}
		else
		if (argument == "-add-to-playlist") {
			add_to_playlist = true;
		}
		else
		if (argument == "-mini" || argument == "-minigui") {
			gui_to_use = "MiniGui";
		}
		else
		if (argument == "-mpcgui") {
			gui_to_use = "MpcGui";
		}
		else
		if (argument == "-defaultgui") {
			gui_to_use = "DefaultGui";
		}
		else {
			// File
			if (QFile::exists( argument )) {
				argument = QFileInfo(argument).absoluteFilePath();
			}
			if (is_playlist) {
				argument = argument + IS_PLAYLIST_TAG;
				is_playlist = false;
			}
			files_to_play.append( argument );
		}
	}

	if (show_help) {
		printf("%s\n", CLHelp::help().toLocal8Bit().data());
		return NoError;
	}

	qDebug("UMPlayer::processArgs: files_to_play: count: %d", files_to_play.count() );
	for (int n=0; n < files_to_play.count(); n++) {
		qDebug("UMPlayer::processArgs: files_to_play[%d]: '%s'", n, files_to_play[n].toUtf8().data());
	}


	if (pref->use_single_instance) {
		// Single instance
		int port = pref->connection_port;
		if (pref->use_autoport) port = pref->autoport;

		MyClient *c = new MyClient(port);
		//c->setTimeOut(1000);
		qDebug("UMPlayer::processArgs: trying to connect to port %d", port);

		if (c->openConnection()) {
			qDebug("UMPlayer::processArgs: found another instance");

			if (!action.isEmpty()) {
				if (c->sendAction(action)) {
					qDebug("UMPlayer::processArgs: action passed successfully to the running instance");
				} else {
					printf("Error: action couldn't be passed to the running instance");
					return NoAction;
				}
			}
			else {
				if (!subtitle_file.isEmpty()) {
					if (c->sendSubtitleFile(subtitle_file)) {
						qDebug("UMPlayer::processArgs: subtitle file sent successfully to the running instance");
					} else {
						qDebug("UMPlayer::processArgs: subtitle file couldn't be sent to another instance");
					}
				}

				if (!files_to_play.isEmpty()) {
					if (c->sendFiles(files_to_play, add_to_playlist)) {
						qDebug("UMPlayer::processArgs: files sent successfully to the running instance");
		    	        qDebug("UMPlayer::processArgs: exiting.");
					} else {
						qDebug("UMPlayer::processArgs: files couldn't be sent to another instance");
					}
				}
                                if (!directory_play.isEmpty()) {
                                        if (c->sendDirectory(directory_play, add_to_playlist)) {
                                                qDebug("UMPlayer::processArgs: directory sent successfully to the running instance");
                                qDebug("UMPlayer::processArgs: exiting.");
                                        } else {
                                                qDebug("UMPlayer::processArgs: directory couldn't be sent to another instance");
                                        }
                                }
			}
			c->closeConnection();
			return NoError;
		} else {
			if (!action.isEmpty()) {
				printf("Error: no running instance found\r\n");
				return NoRunningInstance;
			}
		}
	}

	if (!pref->default_font.isEmpty()) {
		QFont f;
		f.fromString(pref->default_font);
		qApp->setFont(f);
	}

	if (close_at_end != -1) {
		pref->close_on_finish = close_at_end;
	}

	if (start_in_fullscreen != -1) {
		pref->start_in_fullscreen = start_in_fullscreen;
	}

	return UMPlayer::NoExit;
}

void UMPlayer::start() {
	if (!gui()->startHidden() || !files_to_play.isEmpty() ) gui()->show();
	if (!files_to_play.isEmpty()) {
		if (!subtitle_file.isEmpty()) gui()->setInitialSubtitle(subtitle_file);
		gui()->openFiles(files_to_play);
	}
        if( !directory_play.isEmpty()) {
            gui()->openDirectory(directory_play);
        }
	if (!actions_list.isEmpty()) {
		if (files_to_play.isEmpty()) {
			gui()->runActions(actions_list);
		} else {
			gui()->runActionsLater(actions_list);
		}
	}
}

#ifndef PORTABLE_APP
void UMPlayer::createConfigDirectory() {
	// Create umplayer config directory
	if (!QFile::exists(Paths::configPath())) {
		QDir d;
		if (!d.mkdir(Paths::configPath())) {
			qWarning("UMPlayer::createConfigDirectory: can't create %s", Paths::configPath().toUtf8().data());
		}
		QString s = Paths::configPath() + "/screenshots";
		if (!d.mkdir(s)) {
			qWarning("UMPlayer::createHomeDirectory: can't create %s", s.toUtf8().data());
		}
	}
}
#endif

void UMPlayer::showInfo() {
#ifdef Q_OS_WIN
	QString win_ver;
	switch (QSysInfo::WindowsVersion) {
		case QSysInfo::WV_32s: win_ver = "Windows 3.1"; break;
		case QSysInfo::WV_95: win_ver = "Windows 95"; break;
		case QSysInfo::WV_98: win_ver = "Windows 98"; break;
		case QSysInfo::WV_Me: win_ver = "Windows Me"; break;
		case QSysInfo::WV_NT: win_ver = "Windows NT"; break;
		case QSysInfo::WV_2000: win_ver = "Windows 2000"; break;
		case QSysInfo::WV_XP: win_ver = "Windows XP"; break;
		case QSysInfo::WV_2003: win_ver = "Windows Server 2003"; break;
		case QSysInfo::WV_VISTA: win_ver = "Windows Vista"; break;
		#if QT_VERSION >= 0x040501
		case QSysInfo::WV_WINDOWS7: win_ver = "Windows 7"; break;
		#endif
		default: win_ver = QString("other: %1").arg(QSysInfo::WindowsVersion);
	}
#endif
	QString s = QObject::tr("This is UMPlayer v. %1 running on %2")
            .arg(umplayerVersion())
#ifdef Q_OS_LINUX
           .arg("Linux")
#else
#ifdef Q_OS_WIN
           .arg("Windows ("+win_ver+")")
#else
		   .arg("Other OS")
#endif
#endif
           ;

	printf("%s\n", s.toLocal8Bit().data() );
	qDebug("%s", s.toUtf8().data() );
	qDebug("Compiled with Qt v. %s, using %s", QT_VERSION_STR, qVersion());

	qDebug(" * application path: '%s'", Paths::appPath().toUtf8().data());
	qDebug(" * data path: '%s'", Paths::dataPath().toUtf8().data());
	qDebug(" * translation path: '%s'", Paths::translationPath().toUtf8().data());
	qDebug(" * doc path: '%s'", Paths::docPath().toUtf8().data());
	qDebug(" * themes path: '%s'", Paths::themesPath().toUtf8().data());
	qDebug(" * shortcuts path: '%s'", Paths::shortcutsPath().toUtf8().data());
	qDebug(" * config path: '%s'", Paths::configPath().toUtf8().data());
	qDebug(" * ini path: '%s'", Paths::iniPath().toUtf8().data());
	qDebug(" * file for subtitles' styles: '%s'", Paths::subtitleStyleFile().toUtf8().data());
	qDebug(" * current path: '%s'", QDir::currentPath().toUtf8().data());
}
