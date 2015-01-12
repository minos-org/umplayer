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


	prefassociations.cpp
	Handles file associations in Windows
	Author: Florin Braghis (florin@libertv.ro)
*/


#include "prefassociations.h"
#include "images.h"
#include <QSettings>
#include <QApplication>
#include <QMessageBox>
#include "winfileassoc.h"
#include "extensions.h"

static Qt::CheckState CurItemCheckState = Qt::Unchecked; 


PrefAssociations::PrefAssociations(QWidget * parent, Qt::WindowFlags f)
: PrefWidget(parent, f )
{
	setupUi(this);        
	connect(selectAll, SIGNAL(clicked(bool)), this, SLOT(selectAllClicked(bool)));
	connect(selectNone, SIGNAL(clicked(bool)), this, SLOT(selectNoneClicked(bool)));
        connect(restoreDefaults, SIGNAL(clicked(bool)), this, SLOT(restoreDefaultsClicked(bool)));

        connect(listWidgetAudio, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listItemClicked(QListWidgetItem*)));
        connect(listWidgetAudio, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(listItemPressed(QListWidgetItem*)));

        connect(listWidgetVideo, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listItemClicked(QListWidgetItem*)));
        connect(listWidgetVideo, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(listItemPressed(QListWidgetItem*)));

        connect(listWidgetPlaylist, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listItemClicked(QListWidgetItem*)));
        connect(listWidgetPlaylist, SIGNAL(itemPressed(QListWidgetItem*)), this, SLOT(listItemPressed(QListWidgetItem*)));
        connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changeListWidget(int)));

        /*if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
	{
		//Hide Select None - One cannot restore an association in Vista. Go figure.
                selectNone->hide();
		//QPushButton* lpbButton = new QPushButton("Launch Program Defaults", this); 
		//hboxLayout->addWidget(lpbButton); 
		//connect(lpbButton, SIGNAL(clicked(bool)), this, SLOT(launchAppDefaults()));
        }*/

	Extensions e;
        for (int n=0; n < e.audio().count(); n++) {
                addItem( listWidgetAudio, e.audio()[n] );
	}
        for (int n=0; n < e.video().count(); n++) {
                addItem( listWidgetVideo, e.video()[n] );
        }
	// Add the playlist extensions
	for (int n=0; n < e.playlist().count(); n++) {
                addItem( listWidgetPlaylist, e.playlist()[n] );
	}
        changeListWidget(0);
        tabWidget->setCurrentIndex(0);
	retranslateStrings();
}

PrefAssociations::~PrefAssociations()
{

}

void PrefAssociations::selectAllClicked(bool)
{    
	for (int k = 0; k < listWidget->count(); k++)
		listWidget->item(k)->setCheckState(Qt::Checked);
	listWidget->setFocus(); 
}

void PrefAssociations::selectNoneClicked(bool)
{

	for (int k = 0; k < listWidget->count(); k++)
		listWidget->item(k)->setCheckState(Qt::Unchecked);
	listWidget->setFocus(); 
}

void PrefAssociations::restoreDefaultsClicked(bool)
{
    if(listWidget == listWidgetVideo)
    {
        for (int k = 0; k < listWidget->count(); k++)
        {
            QListWidgetItem* item = listWidget->item(k);
            QString text = item->text().toLower();
            if(text == "iso" || text == "bin" || text == "dat")
            {
                item->setCheckState(Qt::Unchecked);
            }
            else
                item->setCheckState(Qt::Checked);
        }
    }
    else
    {
        for (int k = 0; k < listWidget->count(); k++)
        {
            listWidget->item(k)->setCheckState(Qt::Checked);
        }
    }
    listWidget->setFocus();
}

void PrefAssociations::listItemClicked(QListWidgetItem* item)
{
	if (!(item->flags() & Qt::ItemIsEnabled))
		return; 

	if (item->checkState() == CurItemCheckState)
	{
		//Clicked on the list item (not checkbox)
		if (item->checkState() == Qt::Checked)
		{
			item->setCheckState(Qt::Unchecked);
		}
		else
			item->setCheckState(Qt::Checked); 
	}


	//else - clicked on the checkbox itself, do nothing
}

void PrefAssociations::listItemPressed(QListWidgetItem* item)
{
	CurItemCheckState = item->checkState(); 
}

void PrefAssociations::addItem( QListWidget* listWidgetp, QString label)
{
        QListWidgetItem* item = new QListWidgetItem(listWidgetp);
	item->setText(label);
}

void PrefAssociations::refreshList()
{
	m_regExtensions.clear(); 
        WinFileAssoc ().GetRegisteredExtensions(Extensions().allPlayable(), m_regExtensions);

        for (int k = 0; k < listWidgetAudio->count(); k++)
	{
                QListWidgetItem* pItem = listWidgetAudio->item(k);
		if (pItem)
		{
			pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			if (m_regExtensions.contains(pItem->text()))
			{
				pItem->setCheckState(Qt::Checked);
				//Don't allow de-selection in windows VISTA if extension is registered.
				//VISTA doesn't seem to support extension 'restoration' in the API.
                                /*if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA) {
					pItem->setFlags(0);
                                }*/
			}
			else
			{
				pItem->setCheckState(Qt::Unchecked);
			}
		}
	}
        for (int k = 0; k < listWidgetVideo->count(); k++)
        {
                QListWidgetItem* pItem = listWidgetVideo->item(k);
                if (pItem)
                {
                        pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                        if (m_regExtensions.contains(pItem->text()))
                        {
                                pItem->setCheckState(Qt::Checked);
                                /*if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA) {
                                        pItem->setFlags(0);
                                }*/
                        }
                        else
                        {
                                pItem->setCheckState(Qt::Unchecked);
                        }
                }
        }
        for (int k = 0; k < listWidgetPlaylist->count(); k++)
        {
                QListWidgetItem* pItem = listWidgetPlaylist->item(k);
                if (pItem)
                {
                        pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                        if (m_regExtensions.contains(pItem->text()))
                        {
                                pItem->setCheckState(Qt::Checked);
                                /*if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA) {
                                        pItem->setFlags(0);
                                }*/
                        }
                        else
                        {
                                pItem->setCheckState(Qt::Unchecked);
                        }
                }
        }
}

void PrefAssociations::setData(Preferences * )
{
	refreshList(); 
}

int PrefAssociations::ProcessAssociations(QStringList& current, QStringList& old)
{
	WinFileAssoc RegAssoc; 

	QStringList toRestore; 

	//Restore unselected associations
	foreach(const QString& ext, old)
	{
		if (!current.contains(ext))
			toRestore.append(ext); 
	}

	RegAssoc.RestoreFileAssociations(toRestore); 
	return RegAssoc.CreateFileAssociations(current); 
}

void PrefAssociations::getData(Preferences *)
{
	QStringList extensions; 

        for (int k = 0; k < listWidgetAudio->count(); k++)
	{
                QListWidgetItem* pItem = listWidgetAudio->item(k);
		if (pItem && pItem->checkState() == Qt::Checked)
			extensions.append(pItem->text()); 
	}
        for (int k = 0; k < listWidgetVideo->count(); k++)
        {
                QListWidgetItem* pItem = listWidgetVideo->item(k);
                if (pItem && pItem->checkState() == Qt::Checked)
                        extensions.append(pItem->text());
        }
        for (int k = 0; k < listWidgetPlaylist->count(); k++)
        {
                QListWidgetItem* pItem = listWidgetPlaylist->item(k);
                if (pItem && pItem->checkState() == Qt::Checked)
                        extensions.append(pItem->text());
        }

	int processed = ProcessAssociations(extensions, m_regExtensions); 

	if (processed != extensions.count())
	{
		QMessageBox::warning(this, tr("Warning"), 
            tr("Not all files could be associated. Please check your "
               "security permissions and retry."), QMessageBox::Ok);
	}
	
	refreshList(); //Useless when OK is pressed... How to detect if apply or ok is pressed ?
}

QString PrefAssociations::sectionName() {
	return tr("File Types");
}

QPixmap PrefAssociations::sectionIcon() {
	return Images::icon("pref_associations");
}

void PrefAssociations::retranslateStrings() {

	retranslateUi(this);
	createHelp();
}

void PrefAssociations::createHelp() {

	clearHelp();

	setWhatsThis(selectAll, tr("Select all"), 
		tr("Check all file types in the list"));

	setWhatsThis(selectNone, tr("Select none"), 
		tr("Uncheck all file types in the list"));

        setWhatsThis(listWidgetAudio, tr("List of audio file types"),
                tr("Check the audio file extensions you would like UMPlayer to handle. "
		   "When you click Apply, the checked files will be associated with "
                   "UMPlayer. If you uncheck an audio type, the file association will "
		   "be restored.") +
        tr(" <b>Note:</b> (Restoration doesn't work on Windows Vista)."));

        setWhatsThis(listWidgetVideo, tr("List of video file types"),
                tr("Check the video file extensions you would like UMPlayer to handle. "
                   "When you click Apply, the checked files will be associated with "
                   "UMPlayer. If you uncheck a video type, the file association will "
                   "be restored.") +
        tr(" <b>Note:</b> (Restoration doesn't work on Windows Vista)."));

        setWhatsThis(listWidgetPlaylist, tr("List of playlist file types"),
                tr("Check the playlist file extensions you would like UMPlayer to handle. "
                   "When you click Apply, the checked files will be associated with "
                   "UMPlayer. If you uncheck a playlist type, the file association will "
                   "be restored.") +
        tr(" <b>Note:</b> (Restoration doesn't work on Windows Vista)."));
}

void PrefAssociations::changeListWidget(int index)
{
    switch(index)
    {
    case 0:
        listWidget = listWidgetAudio;break;
    case 1:
        listWidget = listWidgetVideo;break;
    case 2:
        listWidget = listWidgetPlaylist;break;
    }
}

#include "moc_prefassociations.cpp"

