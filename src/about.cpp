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

#include "about.h"
#include "images.h"
#include "version.h"
#include "global.h"
#include "preferences.h"
#include "paths.h"
#include "mplayerversion.h"

#include <QFile>

//#define TRANS_ORIG
#define TRANS_LIST
//#define TRANS_TABLE

using namespace Global;

About::About(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setupUi(this);
        QStringList logos;
        logos << "Umplayer-16" << "Umplayer-24" << "Umplayer-32" << "Umplayer-48" << "Umplayer-256" << "Umplayer-512" ;
        setWindowIcon( Images::icon(logos));

        logo->setPixmap( Images::icon("Umplayer-256", 64) );
	license_icon->setPixmap( Images::icon("license" ) );

	QString mplayer_version;
	if (pref->mplayer_detected_version > 0) {
		mplayer_version = tr("Using MPlayer %1").arg(MplayerVersion::toString(pref->mplayer_detected_version)) + "<br><br>";
	}

        info->setHtml(
                QString("<b>" +tr("Version") + "</b>: %1").arg(umplayerVersion()) +
#if PORTABLE_APP
                " (" + tr("Portable Edition") + ")" +
#endif
        "<br>" + QString("<b>" +tr("Compiled") + "</b>: %1").arg(QT_VERSION_STR) +
        "<br><br>" + tr("UMPlayer is an open source multimedia player that fills all your needs,"
        "it can handle any media format and can playback DVDs, (S)VCDs, Audio CDs, TV / Radio"
        "cards and Web streams.") +
        "<br><br>" +tr("UMPlayer is based on ") +link("http://smplayer.sourceforge.net/", "SMPlayer") +
        tr("&copy; 2006 - 2009 Ricardo Villalba and ") + link("http://www.mplayerhq.hu/", "MPlayer") +
        tr("&copy; 2000 - 2010 The MPlayer Project.") + "<br><br>" + tr("This is a custom build for the ") +
        link("https://github.com/minos-org/umplayer", "Minos project"));



        QString license_file = Paths::doc("gpl.txt", pref->language);
	if (QFile::exists(license_file)) {
		QFont fixed_font;
		fixed_font.setStyleHint(QFont::TypeWriter);
		fixed_font.setFamily("Courier");
		license->setFont(fixed_font);

		QFile f(license_file);
		if (f.open(QIODevice::ReadOnly)) {
			license->setText(QString::fromUtf8(f.readAll().constData()));
		}
		f.close();
	} else {
		license->setText(
		"<i>" +
		tr("This program is free software; you can redistribute it and/or modify "
	    "it under the terms of the GNU General Public License as published by "
	    "the Free Software Foundation; either version 2 of the License, or "
  	    "(at your option) any later version.") + "</i>");
	}

	// Copy the background color ("window") of the tab widget to the "base" color of the qtextbrowsers
	// Problem, it doesn't work with some styles, so first we change the "window" color of the tab widgets.
	info_tab->setAutoFillBackground(true);
	license_tab->setAutoFillBackground(true);

	QPalette pal = info_tab->palette();
	pal.setColor(QPalette::Window, palette().color(QPalette::Window) );

	info_tab->setPalette(pal);
	license_tab->setPalette(pal);

	QPalette p = info->palette();
	//p.setBrush(QPalette::Base, info_tab->palette().window());
	p.setColor(QPalette::Base, info_tab->palette().color(QPalette::Window));

	info->setPalette(p);
	//license->setPalette(p);

	adjustSize();
}

About::~About() {
}

QString About::getTranslators() {
	return QString(
                 tr("Founded in February 2005, YouTube&trade; is the world's most popular online "
                    "video community, allowing millions of people to discover, watch and share "
                    "originally-created videos. YouTube&trade; provides a forum for people to "
                    "connect, inform, and inspire others across the globe and acts as a "
                    "distribution platform for original content creators and advertisers large and small.<br><br>"
                    "By using UMPlayer you hereby agree to be bound by Google Terms of Services located "
                    "at ") + link("http://www.google.com/accounts/TOS", "http://www.google.com/accounts/TOS") + "." );

}

QString About::trad(const QString & lang, const QString & author) {
	return trad(lang, QStringList() << author);
}

QString About::trad(const QString & lang, const QStringList & authors) {
#ifdef TRANS_ORIG
	QString s;

	switch (authors.count()) {
		case 2: s = tr("%1 and %2"); break;
		case 3: s = tr("%1, %2 and %3"); break;
		case 4: s = tr("%1, %2, %3 and %4"); break;
		case 5: s = tr("%1, %2, %3, %4 and %5"); break;
		default: s = "%1";
	}

	for (int n = 0; n < authors.count(); n++) {
		QString author = authors[n];
		s = s.arg(author.replace("<", "&lt;").replace(">", "&gt;"));
	}

	return "<li>"+ tr("<b>%1</b>: %2").arg(lang).arg(s) + "</li>";
#endif

#ifdef TRANS_LIST
	QString s = "<ul>";;
	for (int n = 0; n < authors.count(); n++) {
		QString author = authors[n];
		s += "<li>"+ author.replace("<", "&lt;").replace(">", "&gt;") + "</li>";
	}
	s+= "</ul>";

	return "<li>"+ tr("<b>%1</b>: %2").arg(lang).arg(s) + "</li>";
#endif

#ifdef TRANS_TABLE
	QString s;
	for (int n = 0; n < authors.count(); n++) {
		QString author = authors[n];
		s += author.replace("<", "&lt;").replace(">", "&gt;");
		if (n < (authors.count()-1)) s += "<br>";
	}

	return QString("<tr><td align=right><b>%1</b></td><td>%2</td></tr>").arg(lang).arg(s);
#endif
}

QString About::link(const QString & url, QString name) {
	if (name.isEmpty()) name = url;
	return QString("<a href=\"" + url + "\">" + name +"</a>");
}

QString About::contr(const QString & author, const QString & thing) {
	return "<li>"+ tr("<b>%1</b> (%2)").arg(author).arg(thing) +"</li>";
}

QSize About::sizeHint () const {
	return QSize(518, 326);
}

#include "moc_about.cpp"
