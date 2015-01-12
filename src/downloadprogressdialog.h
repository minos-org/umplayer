/*  umplayer, GUI front-end for mplayer.
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

#ifndef DOWNLOADBINARYDIALOG_H
#define DOWNLOADBINARYDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QDialog>
#include <QLabel>
#include <QTime>
#include <QQueue>
#include <QPair>
#include <QTemporaryFile>


class DownloadBinaryDialog : public QDialog
{
Q_OBJECT
public:
    explicit DownloadBinaryDialog(QString site, QWidget *parent = 0);


private:
    QLabel* headerLabel;
    QLabel* footerLabel;
    QProgressBar* progressBar;

    quint64 totalSize;
    quint64 completed;
    quint64 speed;

    QString url;    

    QQueue< QPair<QTime, qint64> > lastReceivedBytes;
    QTemporaryFile* tempFile;
    void updateFooterText();


signals:
    void programStarted(bool);
    void downloadFinished(bool);
    void errorOcurred();

public slots:
    void setTitle(int percent);
    void startBinary(bool error);


};




#endif // DOWNLOADBINARYDIALOG_H
