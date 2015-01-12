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

#include "downloadprogressdialog.h"
#include "global.h"
#include "downloadfile.h"
#include "preferences.h"
#include <QBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTemporaryFile>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#define MOD_TIME(x) ((x+86400000)%86400000)

using namespace Global;

DownloadBinaryDialog::DownloadBinaryDialog(QString site, QWidget *parent):
    QDialog(parent), speed(0)
{
    url = site;    
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    setAttribute(Qt::WA_DeleteOnClose, true);
    resize(420, 90);
    headerLabel = new QLabel("Please wait for the download to finish");
    QFont font = headerLabel->font();
    font.setPixelSize(14);
    headerLabel->setFont(font);
    footerLabel = new QLabel(this);
    font.setPixelSize(12);
    footerLabel->setFont(font);
    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);
    progressBar->setRange(0, 100);
    QVBoxLayout* vbox = new QVBoxLayout;
    vbox->addWidget(headerLabel);
    vbox->addWidget(progressBar);
    vbox->addWidget(footerLabel);
    setLayout(vbox);
    connect(progressBar, SIGNAL(valueChanged(int)), this, SLOT(setTitle(int)));
    if(url.endsWith('/')) url.chop(1);
    QString fileName = url.mid(url.lastIndexOf('/'));
    QStringList fileNameSections = fileName.split('.');
    fileNameSections[0] += "_XXXXXX";
    tempFile = new QTemporaryFile(QDir::tempPath() + fileNameSections.join("."), this);
    tempFile->setAutoRemove(false);
    DownloadFile* df = new DownloadFile(url, tempFile, this  );
    connect(df, SIGNAL(errorOcurred(int)), this, SIGNAL(errorOcurred()));
    connect(df, SIGNAL(errorOcurred(int)), this, SLOT(close()));
    connect(df, SIGNAL(downloadStatus(QString)), footerLabel, SLOT(setText(QString)) );
    connect(df, SIGNAL(progress(int, qint64)), progressBar, SLOT(setValue(int)));
    connect(df, SIGNAL(downloadFinished(bool)), this, SLOT(startBinary(bool)));
    setWindowTitle("Update in Progress");
}

void DownloadBinaryDialog::setTitle(int percent)
{
    setWindowTitle(QString("%1% - Update in Progress").arg(percent));
}

void DownloadBinaryDialog::startBinary(bool error)
{
    if(error) return;
    tempFile->setPermissions(tempFile->permissions() | QFile::ExeOwner);
    QFileInfo fileInfo(*tempFile);
    QString prog = QDir::toNativeSeparators(fileInfo.absoluteFilePath());
    delete tempFile;
    if(QProcess::startDetached(prog))
    {
        pref->lastUpdateTime = QDateTime::currentDateTime();
        pref->save();
        emit programStarted(true);
    }
}

