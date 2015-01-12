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

#ifndef _SMPLAYER_H_
#define _SMPLAYER_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include <QApplication>
#include "basegui.h"



class MyApplication : public QApplication
{
    Q_OBJECT
public:
        MyApplication ( int & argc, char ** argv ) : QApplication(argc, argv) {
#ifdef Q_OS_MACX
            connect(this, SIGNAL(focusChanged(QWidget*,QWidget*)), SLOT(newFocus(QWidget*,QWidget*)));
#endif
        }
        virtual void commitData ( QSessionManager & /*manager*/ ) {
                // Nothing to do, let the application to close
        }
private slots:
#ifdef Q_OS_MACX
        void newFocus (QWidget* , QWidget* newOne)
        {
            if(newOne && newOne->testAttribute(Qt::WA_MacShowFocusRect))
                newOne->setAttribute(Qt::WA_MacShowFocusRect, false);
        }
#endif
};



class UMPlayer : public QObject
{
    Q_OBJECT

public:
	enum ExitCode { ErrorArgument = -3, NoAction = -2, NoRunningInstance = -1, NoError = 0, NoExit = 1 };

	UMPlayer(const QString & config_path = QString::null, QObject * parent = 0);
	~UMPlayer();

	//! Process arguments. If ExitCode != NoExit the application must be exited.
	ExitCode processArgs(QStringList args);

	BaseGui * gui();

        BaseGui * gui(QString skin);

	void start();
        void setBasegui_instance(BaseGui** ins) { basegui_instance = ins; }


private:
#ifndef PORTABLE_APP
	void createConfigDirectory();
#endif
	void showInfo();


	BaseGui * main_window;
        BaseGui ** basegui_instance;

    QStringList files_to_play;
    QString directory_play;
    QString subtitle_file;
    QString actions_list; //!< Actions to be run on startup
	QString gui_to_use;

	// Change position and size
	bool move_gui;
	QPoint gui_position;

	bool resize_gui;
	QSize gui_size;

private slots:
        void changeSkin();
        void baseGuiDestroyed(QObject* destroyed);

};

#endif
