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

#include "qpropertysetter.h"
#include "myicon.h"
#include "images.h"


#define ICON_ADD(icon, png) { icon.addPixmap(Images::icon(png, 16), QIcon::Normal, QIcon::Off); \
                              icon.addPixmap(Images::icon(png, 16), QIcon::Active, QIcon::Off); \
                              icon.addPixmap(Images::icon(png, 16), QIcon::Selected, QIcon::Off); \
                              icon.addPixmap(Images::icon(png, 16), QIcon::Disabled, QIcon::Off);}


IconSetter* IconSetter::m_instance = 0;

IconSetter::IconSetter(QWidget *parent) :
    QWidget(parent)
{
    logoBgPix = Images::icon("ump-splash-bg");
    logoSplashPix = Images::icon("ump-splash-highlight");
    logoPix = Images::icon("ump-splash-logo");
}

IconSetter* IconSetter::instance()
{
    if(m_instance == 0)
    {
        m_instance = new IconSetter();
    }
    return m_instance;
}

void IconSetter::removeInstance()
{
    if(m_instance)
        delete m_instance;
    m_instance = 0;
}

void IconSetter::setActionIcon(QPixmap pixmap )
{
    for(int n = 0; n < 10; ++n )
    {
        QIcon icon;
        icon.addPixmap(pixmap.copy(n*24, 0, 24, 24), QIcon::Normal, QIcon::Off);
        icon.addPixmap(pixmap.copy(n*24, 24, 24, 24), QIcon::Active, QIcon::Off);
        icon.addPixmap(pixmap.copy(n*24, 48, 24, 24), QIcon::Selected, QIcon::Off);
        icon.addPixmap(pixmap.copy(n*24, 72, 24, 24), QIcon::Disabled, QIcon::Off);
        QAction * action = 0;
        switch(n)
        {
        case 0: action = openFileAct;
                ICON_ADD(icon, "file")
                break;
        case 1: action = openDirectoryAct;
                ICON_ADD(icon, "folder")
                break;
        case 2: action = openDVDAct;
                ICON_ADD(icon, "dvd_drive")
                break;
        case 3: action = openURLAct;
                ICON_ADD(icon, "url")
                break;
        case 4: action = screenshotAct;
                ICON_ADD(icon, "screenshot")
                break;
        case 5: action = showPropertiesAct;
                ICON_ADD(icon, "info")
                break;
        case 6: action = showFindSubtitlesDialogAct;
                ICON_ADD(icon, "download_subs")
                break;
        case 7: action = showPreferencesAct;
                ICON_ADD(icon, "preferences")
                break;
        case 8:
                action = youtubeAct;
                ICON_ADD(icon, "youtube-16")
                break;
        case 9:
                action = shoutcastAct;
                ICON_ADD(icon, "shoutcast")
                break;
        }
        action->setIcon(icon);
    }
}

void IconSetter::buttonIcon(int buttonNo, QPixmap pix )
{
    MyIcon icon;
    int w = pix.width();
    int h = pix.height();
    icon.setPixmap(pix.copy(0, 0, w, h/4 ), MyIcon::Normal, MyIcon::Off);
    icon.setPixmap(pix.copy(0, h/4, w, h/4 ), MyIcon::MouseOver, MyIcon::Off);
    icon.setPixmap(pix.copy(0, h/2, w, h/4 ), MyIcon::MouseDown, MyIcon::Off);
    icon.setPixmap(pix.copy(0, 3*h/4, w, h/4 ), MyIcon::Disabled, MyIcon::Off);
    MyIcon icon2;
    switch(buttonNo)
    {
    case 1:
        playControl->setBackwardIcon(icon);
        break;
    case 2:
        playControl->setPreviousIcon(icon);break;
    case 3:        
        icon2.setPixmap(pix.copy(0, 0, w/2, h/4 ), MyIcon::Normal, MyIcon::Off);
        icon2.setPixmap(pix.copy(0, h/4, w/2, h/4 ), MyIcon::MouseOver, MyIcon::Off);
        icon2.setPixmap(pix.copy(0, h/2, w/2, h/4 ), MyIcon::MouseDown, MyIcon::Off);
        icon2.setPixmap(pix.copy(0, 3*h/4, w/2, h/4 ), MyIcon::Disabled, MyIcon::Off);

        icon2.setPixmap(pix.copy(w/2, 0, w/2, h/4 ), MyIcon::Normal, MyIcon::On);
        icon2.setPixmap(pix.copy(w/2, h/4, w/2, h/4 ), MyIcon::MouseOver, MyIcon::On);
        icon2.setPixmap(pix.copy(w/2, h/2, w/2, h/4 ), MyIcon::MouseDown, MyIcon::On);
        icon2.setPixmap(pix.copy(w/2, 3*h/4, w/2, h/4 ), MyIcon::Disabled, MyIcon::On);

        playControl->setPlayPauseIcon(icon2);
        break;
    case 4:
        playControl->setStopIcon(icon);break;
    case 5:
        playControl->setRecordIcon(icon);break;
    case 6:
        playControl->setNextIcon(icon);break;
    case 7:
        playControl->setForwardIcon(icon);break;

    }
}


void IconSetter::mediaPanelButtonIcon( int n, QPixmap pix)
{
    if(pix.isNull()) return;
    MyIcon icon;
    int w = pix.width();
    int h = pix.height();
    icon.setPixmap(pix.copy(0, 0, w/2, h/4 ), MyIcon::Normal, MyIcon::Off);
    icon.setPixmap(pix.copy(0, h/4, w/2, h/4 ), MyIcon::MouseOver, MyIcon::Off);
    icon.setPixmap(pix.copy(0, h/2, w/2, h/4 ), MyIcon::MouseDown, MyIcon::Off);
    icon.setPixmap(pix.copy(0, 3*h/4, w/2, h/4 ), MyIcon::Disabled, MyIcon::Off);

    icon.setPixmap(pix.copy(w/2, 0, w/2, h/4 ), MyIcon::Normal, MyIcon::On);
    icon.setPixmap(pix.copy(w/2, h/4, w/2, h/4 ), MyIcon::MouseOver, MyIcon::On);
    icon.setPixmap(pix.copy(w/2, h/2, w/2, h/4 ), MyIcon::MouseDown, MyIcon::On);
    icon.setPixmap(pix.copy(w/2, 3*h/4, w/2, h/4 ), MyIcon::Disabled, MyIcon::On);

    switch(n)
    {
    case 1:
        mediaPanel->setShuffleIcon(icon);break;
    case 2:
        mediaPanel->setRepeatIcon(icon);break;
    }
}
