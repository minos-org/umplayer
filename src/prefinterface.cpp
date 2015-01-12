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


#include "prefinterface.h"
#include "images.h"
#include "preferences.h"
#include "paths.h"
#include "config.h"
#include "languages.h"
#include "recents.h"
#include "helper.h"

#include <QDir>
#include <QFontDialog>

PrefInterface::PrefInterface(QWidget * parent, Qt::WindowFlags f)
	: PrefWidget(parent, f )
{
	setupUi(this);
	/* volume_icon->hide(); */

        // Skin combo
        skin_combo->addItem( "Do no use skins" );

	// User
        QDir skin_dir = Paths::configPath() + "/themes";
        qDebug("skin_dir: %s", skin_dir.absolutePath().toUtf8().data());
        QStringList skins = skin_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int n=0; n < skins.count(); n++) {
                skin_combo->addItem( skins[n] );
	}
	// Global
        skin_dir = Paths::themesPath();
        qDebug("skin_dir: %s", skin_dir.absolutePath().toUtf8().data());
        skins = skin_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int n=0; n < skins.count(); n++) {
                if (skin_combo->findText( skins[n] ) == -1) {
                        skin_combo->addItem( skins[n] );
		}
	}

	connect(single_instance_check, SIGNAL(toggled(bool)), 
            this, SLOT(changeInstanceImages()));

//#ifdef Q_OS_WIN
	//floating_bypass_wm_check->hide();
//#endif

	retranslateStrings();
}

PrefInterface::~PrefInterface()
{
}

QString PrefInterface::sectionName() {
	return tr("Interface");
}

QPixmap PrefInterface::sectionIcon() {
    return Images::icon("pref_gui");
}

void PrefInterface::createLanguageCombo()
{
    QMap <QString,QString> m = Helper::interfaceLanguages();
    language_combo->clear();
    language_combo->addItem( tr("<Autodetect>") );
    foreach(QString key, m.keys())
    {
        language_combo->addItem(key, m.value(key));
    }
}

void PrefInterface::retranslateStrings() {
	int mainwindow_resize = mainwindow_resize_combo->currentIndex();
	int timeslider_pos = timeslider_behaviour_combo->currentIndex();

	retranslateUi(this);

	mainwindow_resize_combo->setCurrentIndex(mainwindow_resize);
	timeslider_behaviour_combo->setCurrentIndex(timeslider_pos);

	// Icons
	resize_window_icon->setPixmap( Images::icon("resize_window") );
	/* volume_icon->setPixmap( Images::icon("speaker") ); */

	changeInstanceImages();

	// Seek widgets
	seek1->setLabel( tr("&Short jump") );
	seek2->setLabel( tr("&Medium jump") );
	seek3->setLabel( tr("&Long jump") );
	seek4->setLabel( tr("Mouse &wheel jump") );

	if (qApp->isLeftToRight()) {
                seek1->setIcon( Images::icon("short") );
                seek2->setIcon( Images::icon("medium") );
                seek3->setIcon( Images::icon("long") );
	} else {
                seek1->setIcon( Images::flippedIcon("short") );
                seek2->setIcon( Images::flippedIcon("medium") );
                seek3->setIcon( Images::flippedIcon("long") );
	}
        seek4->setIcon( Images::icon("seek_mouse", seek1->icon()->width()) );

	// Language combo
	int language_item = language_combo->currentIndex();
	createLanguageCombo();
	language_combo->setCurrentIndex( language_item );

        // Skin combo
        skin_combo->setItemText(0, tr("Do no use skins"));


	//floating_width_label->setNum(floating_width_slider->value());
	//floating_margin_label->setNum(floating_margin_slider->value());

	createHelp();
}

void PrefInterface::setData(Preferences * pref) {
	setLanguage( pref->language );
        setSkin( pref->skin );

	setResizeMethod( pref->resize_method );
	setSaveSize( pref->save_window_size_on_exit );
	setUseSingleInstance(pref->use_single_instance);
	setServerPort(pref->connection_port);
	setUseAutoPort(pref->use_autoport);
	setRecentsMaxItems(pref->history_recents->maxItems());

	setSeeking1(pref->seeking1);
	setSeeking2(pref->seeking2);
	setSeeking3(pref->seeking3);
	setSeeking4(pref->seeking4);

	setUpdateWhileDragging(pref->update_while_seeking);

	setDefaultFont(pref->default_font);


	//setFloatingAnimated(pref->floating_control_animated);
	//setFloatingWidth(pref->floating_control_width);
	//setFloatingMargin(pref->floating_control_margin);

//#ifndef Q_OS_WIN
	//setFloatingBypassWindowManager(pref->bypass_window_manager);
//#endif
}

void PrefInterface::getData(Preferences * pref) {
	requires_restart = false;
	language_changed = false;
        skin_changed = false;
	recents_changed = false;
	port_changed = false;


	if (pref->language != language()) {
		pref->language = language();
		language_changed = true;
		qDebug("PrefInterface::getData: chosen language: '%s'", pref->language.toUtf8().data());
	}

        if (pref->skin != skin()) {
                pref->skin = skin();
                skin_changed = true;
	}

	pref->resize_method = resizeMethod();
	pref->save_window_size_on_exit = saveSize();

	pref->use_single_instance = useSingleInstance();
	if (pref->connection_port != serverPort()) {
		pref->connection_port = serverPort();
		port_changed = true;
	}

	if (pref->use_autoport != useAutoPort()) {
		pref->use_autoport = useAutoPort();
		port_changed = true;
	}

	if (pref->history_recents->maxItems() != recentsMaxItems()) {
		pref->history_recents->setMaxItems( recentsMaxItems() );
		recents_changed = true;
	}

	pref->seeking1 = seeking1();
	pref->seeking2 = seeking2();
	pref->seeking3 = seeking3();
	pref->seeking4 = seeking4();

	pref->update_while_seeking = updateWhileDragging();

	pref->default_font = defaultFont();


	//pref->floating_control_animated = floatingAnimated();
	//pref->floating_control_width = floatingWidth();
	//pref->floating_control_margin = floatingMargin();

//#ifndef Q_OS_WIN
	//pref->bypass_window_manager = floatingBypassWindowManager();
//#endif
}

void PrefInterface::setLanguage(QString lang) {
	if (lang.isEmpty()) {
		language_combo->setCurrentIndex(0);
	}
	else {
		int pos = language_combo->findData(lang);
		if (pos != -1) 
			language_combo->setCurrentIndex( pos );
		else
			language_combo->setCurrentText(lang);
	}
}

QString PrefInterface::language() {
	if (language_combo->currentIndex()==0) 
		return "";
	else 
		return language_combo->itemData( language_combo->currentIndex() ).toString();
}

void PrefInterface::setSkin(QString skin){
    if(skin.isEmpty())
        skin_combo->setCurrentIndex(0);
    else
        skin_combo->setCurrentText(skin);
}

QString PrefInterface::skin(){
    if(skin_combo->currentIndex() == 0)
        return "";
    else
        return skin_combo->currentText();
}

void PrefInterface::setResizeMethod(int v) {
	mainwindow_resize_combo->setCurrentIndex(v);
}

int PrefInterface::resizeMethod() {
	return mainwindow_resize_combo->currentIndex();
}

void PrefInterface::setSaveSize(bool b) {
	save_size_check->setChecked(b);
}

bool PrefInterface::saveSize() {
	return save_size_check->isChecked();
}


void PrefInterface::setUseSingleInstance(bool b) {
	single_instance_check->setChecked(b);
	//singleInstanceButtonToggled(b);
}

bool PrefInterface::useSingleInstance() {
	return single_instance_check->isChecked();
}

void PrefInterface::setServerPort(int port) {
	server_port_spin->setValue(port);
}

int PrefInterface::serverPort() {
	return server_port_spin->value();
}

void PrefInterface::setUseAutoPort(bool b) {
	automatic_port_button->setChecked(b);
	manual_port_button->setChecked(!b);
}

bool PrefInterface::useAutoPort() {
	return automatic_port_button->isChecked();
}

void PrefInterface::setRecentsMaxItems(int n) {
	recents_max_items_spin->setValue(n);
}

int PrefInterface::recentsMaxItems() {
	return recents_max_items_spin->value();
}

void PrefInterface::setSeeking1(int n) {
	seek1->setTime(n);
}

int PrefInterface::seeking1() {
	return seek1->time();
}

void PrefInterface::setSeeking2(int n) {
	seek2->setTime(n);
}

int PrefInterface::seeking2() {
	return seek2->time();
}

void PrefInterface::setSeeking3(int n) {
	seek3->setTime(n);
}

int PrefInterface::seeking3() {
	return seek3->time();
}

void PrefInterface::setSeeking4(int n) {
	seek4->setTime(n);
}

int PrefInterface::seeking4() {
	return seek4->time();
}

void PrefInterface::setUpdateWhileDragging(bool b) {
	if (b) 
		timeslider_behaviour_combo->setCurrentIndex(0);
	else
		timeslider_behaviour_combo->setCurrentIndex(1);
}

bool PrefInterface::updateWhileDragging() {
	return (timeslider_behaviour_combo->currentIndex() == 0);
}

void PrefInterface::setDefaultFont(QString font_desc) {
	default_font_edit->setText(font_desc);
}

QString PrefInterface::defaultFont() {
	return default_font_edit->text();
}

void PrefInterface::on_changeFontButton_clicked() {
	QFont f = qApp->font();

	if (!default_font_edit->text().isEmpty()) {
		f.fromString(default_font_edit->text());
	}

	bool ok;
	f = QFontDialog::getFont( &ok, f, this);

	if (ok) {
		default_font_edit->setText( f.toString() );
	}
}

void PrefInterface::changeInstanceImages() {

}

// Floating tab
//void PrefInterface::setFloatingAnimated(bool b) {
	//floating_animated_check->setChecked(b);
//}

//bool PrefInterface::floatingAnimated() {
	//return floating_animated_check->isChecked();
//}

//void PrefInterface::setFloatingWidth(int percentage) {
	//floating_width_slider->setValue(percentage);
//}

//int PrefInterface::floatingWidth() {
	//return floating_width_slider->value();
//}

//void PrefInterface::setFloatingMargin(int pixels) {
	//floating_margin_slider->setValue(pixels);
//}

//int PrefInterface::floatingMargin() {
	//return floating_margin_slider->value();
//}


//#ifndef Q_OS_WIN
//void PrefInterface::setFloatingBypassWindowManager(bool b) {
	//floating_bypass_wm_check->setChecked(b);
//}

//bool PrefInterface::floatingBypassWindowManager() {
	//return floating_bypass_wm_check->isChecked();
//}
//#endif

void PrefInterface::createHelp() {
	clearHelp();

	addSectionTitle(tr("Interface"));

	setWhatsThis(mainwindow_resize_combo, tr("Autoresize"),
        tr("The main window can be resized automatically. Select the option "
           "you prefer.") );

	setWhatsThis(save_size_check, tr("Remember position and size"),
        tr("If you check this option, the position and size of the main "
           "window will be saved and restored when you run UMPlayer again.") );

	setWhatsThis(recents_max_items_spin, tr("Recent files"),
        tr("Select the maximum number of items that will be shown in the "
           "<b>Open->Recent files</b> submenu. If you set it to 0 that "
           "menu won't be shown at all.") );

	setWhatsThis(language_combo, tr("Language"),
		tr("Here you can change the language of the application.") );

        setWhatsThis(skin_combo, tr("Icon set"),
        tr("Select the skin you prefer for the application.") );



	setWhatsThis(changeFontButton, tr("Default font"),
        tr("You can change here the application's font.") );

	addSectionTitle(tr("Seeking"));

	setWhatsThis(seek1, tr("Short jump"),
        tr("Select the time that should be go forward or backward when you "
           "choose the %1 action.").arg(tr("short jump")) );

	setWhatsThis(seek2, tr("Medium jump"),
        tr("Select the time that should be go forward or backward when you "
           "choose the %1 action.").arg(tr("medium jump")) );

	setWhatsThis(seek3, tr("Long jump"),
        tr("Select the time that should be go forward or backward when you "
           "choose the %1 action.").arg(tr("long jump")) );

	setWhatsThis(seek4, tr("Mouse wheel jump"),
        tr("Select the time that should be go forward or backward when you "
           "move the mouse wheel.") );

	setWhatsThis(timeslider_behaviour_combo, tr("Behaviour of time slider"),
        tr("Select what to do when dragging the time slider.") );

	addSectionTitle(tr("Instances"));

	setWhatsThis(single_instance_check, 
        tr("Use only one running instance of UMPlayer"),
        tr("Check this option if you want to use an already running instance "
           "of UMPlayer when opening other files.") );

	setWhatsThis(automatic_port_button, tr("Automatic port"),
        tr("UMPlayer needs to listen to a port to receive commands from other "
           "instances. If you select this option, a port will be "
           "automatically chosen.") );

	setWhatsThis(server_port_spin, tr("Manual port"),
        tr("UMPlayer needs to listen to a port to receive commands from other "
           "instances. You can change the port in case the default one is "
           "used by another application.") );

	manual_port_button->setWhatsThis( server_port_spin->whatsThis() );

	//addSectionTitle(tr("Floating control"));

	//setWhatsThis(floating_animated_check, tr("Animated"),
		//tr("If this option is enabled, the floating control will appear "
           //"with an animation.") );

	//setWhatsThis(floating_width_slider, tr("Width"),
		//tr("Specifies the width of the control (as a percentage).") );

	//setWhatsThis(floating_margin_slider, tr("Margin"),
		//tr("This option sets the number of pixels that the floating control "
           //"will be away from the bottom of the screen. Useful when the "
           //"screen is a TV, as the overscan might prevent the control to be "
           //"visible.") );


//#ifndef Q_OS_WIN
	//setWhatsThis(floating_bypass_wm_check, tr("Bypass window manager"),
		//tr("If this option is checked, the control is displayed bypassing the "
           //"window manager. Disable this option if the floating control "
           //"doesn't work well with your window manager.") );
//#endif
}

#include "moc_prefinterface.cpp"
