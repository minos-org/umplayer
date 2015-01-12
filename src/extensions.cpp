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

#include "extensions.h"

ExtensionList::ExtensionList() : QStringList()
{
}

QString ExtensionList::forFilter() {
	QString s;
	for (int n=0; n < count(); n++) {
		s = s + "*." + at(n) + " ";
	}
	if (!s.isEmpty()) s = " (" + s + ")";
	return s;
}

QString ExtensionList::forRegExp() {
	QString s;
	for (int n=0; n < count(); n++) {
		if (!s.isEmpty()) s = s + "|";
		s = s + "^" + at(n) + "$";
	}
	return s;
}

Extensions::Extensions()
{
        _video  <<  "26l"
                <<  "3gp"
                <<  "asf"
                <<  "avi"
                <<  "bin"
                <<  "dat"
                <<  "divx"
                <<  "dv"
                <<  "evo"
                <<  "flv"
                <<  "iso"
                <<  "jsv"
                <<  "m1v"
                <<  "m2p"
                <<  "m2ts"
                <<  "m2v"
                <<  "m4v"
                <<  "mkv"
                <<  "mov"
                <<  "mp4"
                <<  "mpe"
                <<  "mpeg"
                <<  "mpg"
                <<  "mpv"
                <<  "mqv"
                <<  "nsv"
                <<  "ogm"
                <<  "pva"
                <<  "qt"
                <<  "ra"
                <<  "ram"
                <<  "rm"
                <<  "rmvb"
                <<  "ts"
                <<  "vcd"
                <<  "vfw"
                <<  "vob"
                <<  "wmv" ;


        _audio  <<  "aac"
                <<  "ac3"
                <<  "aiff"
                <<  "ape"
                <<  "dts"
                <<  "fla"
                <<  "flac"
                <<  "m4a"
                <<  "mka"
                <<  "mp1"
                <<  "mp2"
                <<  "mp3"
                <<  "mp+"
                <<  "mpa"
                <<  "mpc"
                <<  "mpp"
                <<  "nsa"
                <<  "ogg"
                <<  "shn"
                <<  "spx"
                <<  "tak"
                <<  "tta"
                <<  "wav"
                <<  "wma"
                <<  "wv" ;


	_subtitles << "srt" << "sub" << "ssa" << "ass" << "idx" << "txt" << "smi"
               << "rt" << "utf" << "aqt";

        _playlist  <<  "asx"
                    <<  "fpl"
                    <<  "m3u"
                    <<  "mpcpl"
                    <<  "pls"
                    <<  "ram"
                    <<  "wpl" ;

	_multimedia = _video;
	for (int n = 0; n < _audio.count(); n++) {
		if (!_multimedia.contains(_audio[n])) _multimedia << _audio[n];
	}

	_all_playable << _multimedia << _playlist;
}

Extensions::~Extensions() {
}

