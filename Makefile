
PREFIX=/usr/local
#PREFIX=/tmp/umplayer

DEFINES       = -DUNICODE -DQT_LARGEFILE_SUPPORT -DQT_DLL -DQT_CORE_LIB -DQT_THREAD_SUPPORT
CFLAGS        = -g -Wall $(DEFINES)
CXXFLAGS      = -g -Wall -frtti -fexceptions -mthreads $(DEFINES)
CONF_PREFIX=$(PREFIX)

DATA_PATH=$(PREFIX)/share/umplayer
DOC_PATH=$(PREFIX)/share/doc/packages/umplayer
TRANSLATION_PATH=$(PREFIX)/share/umplayer/translations
THEMES_PATH=$(PREFIX)/share/umplayer/themes
SHORTCUTS_PATH=$(PREFIX)/share/umplayer/shortcuts

#KDE_PREFIX=`kde-config --prefix`
#KDE_PREFIX=/tmp/umplayer/kde/
KDE_PREFIX=$(PREFIX)

KDE_ICONS=$(KDE_PREFIX)/share/icons/hicolor/
KDE_APPLNK=$(KDE_PREFIX)/share/applications/

QMAKE=qmake
LRELEASE=ldebug

DEFS=DATA_PATH=\\\"$(DATA_PATH)\\\" \
     TRANSLATION_PATH=\\\"$(TRANSLATION_PATH)\\\" \
     DOC_PATH=\\\"$(DOC_PATH)\\\" THEMES_PATH=\\\"$(THEMES_PATH)\\\" \
     SHORTCUTS_PATH=\\\"$(SHORTCUTS_PATH)\\\"


ifdef KDE_SUPPORT

# KDE paths, change if necessary

KDE_INCLUDE_PATH=`kde-config --prefix`/include/
KDE_LIB_PATH=`kde-config --prefix`/lib/
KDE_LIBS=-lkio

QMAKE_OPTS=DEFINES+=KDE_SUPPORT INCLUDEPATH+=$(KDE_INCLUDE_PATH) \
           LIBS+="$(KDE_LIBS) -L$(KDE_LIB_PATH)"

endif

src/umplayer:
	+cd src && $(QMAKE) $(QMAKE_OPTS) && $(DEFS) make
	cd src && $(LRELEASE) umplayer.pro

clean:
	cd src && make distclean
	-rm src/translations/umplayer_*.qm

install: src/umplayer
	-install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 src/umplayer $(DESTDIR)$(PREFIX)/bin/
	-install -d $(DESTDIR)$(DATA_PATH)
	install -m 644 src/input.conf $(DESTDIR)$(DATA_PATH)
	-install -d $(DESTDIR)$(TRANSLATION_PATH)
	install -m 644 src/translations/*.qm $(DESTDIR)$(TRANSLATION_PATH)
	-install -d $(DESTDIR)$(DOC_PATH)
	install -m 644 Changelog *.txt $(DESTDIR)$(DOC_PATH)

	-install -d $(DESTDIR)$(DOC_PATH)
	tar -C docs/ --exclude=.svn -c -f - . | tar -C $(DESTDIR)$(DOC_PATH) -x -f -

	-install -d $(DESTDIR)$(SHORTCUTS_PATH)
	cp src/shortcuts/* $(DESTDIR)$(SHORTCUTS_PATH)
	-install -d $(DESTDIR)$(THEMES_PATH)
	cp -a src/themes/* $(DESTDIR)$(THEMES_PATH)

	-install -d $(DESTDIR)$(KDE_ICONS)/64x64/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/32x32/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/22x22/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/16x16/apps/
	install -m 644 icons/umplayer_icon64.png $(DESTDIR)$(KDE_ICONS)/64x64/apps/umplayer.png
	install -m 644 icons/umplayer_icon32.png $(DESTDIR)$(KDE_ICONS)/32x32/apps/umplayer.png
	install -m 644 icons/umplayer_icon22.png $(DESTDIR)$(KDE_ICONS)/22x22/apps/umplayer.png
	install -m 644 icons/umplayer_icon16.png $(DESTDIR)$(KDE_ICONS)/16x16/apps/umplayer.png
	-install -d $(DESTDIR)$(KDE_APPLNK)
	install -m 644 umplayer.desktop $(DESTDIR)$(KDE_APPLNK)
	install -m 644 umplayer_enqueue.desktop $(DESTDIR)$(KDE_APPLNK)
	-install -d $(DESTDIR)$(PREFIX)/share/man/man1/
	install -m 644 man/umplayer.1 $(DESTDIR)$(PREFIX)/share/man/man1/
	gzip -9 -f $(DESTDIR)$(PREFIX)/share/man/man1/umplayer.1

uninstall:
	-rm -f $(PREFIX)/bin/umplayer
	-rm -f $(DATA_PATH)/input.conf
	-rm -f $(TRANSLATION_PATH)/*.qm
	-rm -f $(DOC_PATH)/Changelog
	-rm -f $(DOC_PATH)/*.txt
	-rm -f $(SHORTCUTS_PATH)/*.keys
	-rm -f $(KDE_ICONS)/64x64/apps/umplayer.png
	-rm -f $(KDE_ICONS)/32x32/apps/umplayer.png
	-rm -f $(KDE_ICONS)/22x22/apps/umplayer.png
	-rm -f $(KDE_ICONS)/16x16/apps/umplayer.png
	-rm -f $(KDE_APPLNK)/umplayer.desktop
	-rm -f $(PREFIX)/share/man/man1/umplayer.1.gz
	-rmdir $(SHORTCUTS_PATH)/
	-rmdir $(TRANSLATION_PATH)/
#	-for file in docs/*/*; do \
#	    rm -f $(DOC_PATH)/$${file/docs/}; \
#	done;
#	-for file in docs/*; do \
#	    rmdir $(DOC_PATH)/$${file/docs/}; \
#	done;
	-(cd docs && find -iname '*.html') | (cd $(DESTDIR)$(DOC_PATH) && xargs rm)
	-(cd docs && find -type d -name '??') | (cd $(DESTDIR)$(DOC_PATH) && xargs rmdir)
	-rmdir $(DOC_PATH)/
	-rmdir $(DATA_PATH)/

