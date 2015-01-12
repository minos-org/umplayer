TEMPLATE = app
LANGUAGE = C++
CONFIG += qt warn_on debug
QT += network \
    xml
RESOURCES = icons.qrc
INCLUDEPATH += findsubtitles \
    videopreview \
    mpcgui
DEPENDPATH += findsubtitles \
    videopreview \
    mpcgui

# DEFINES += USE_QXT
DEFINES += DOWNLOAD_SUBS
HEADERS += mediapanel.h \
    guiconfig.h \
    config.h \
    constants.h \
    svn_revision.h \
    version.h \
    global.h \
    paths.h \
    helper.h \
    colorutils.h \
    translator.h \
    subtracks.h \
    tracks.h \
    titletracks.h \
    discname.h \
    extensions.h \
    desktopinfo.h \
    myprocess.h \
    mplayerversion.h \
    mplayerprocess.h \
    infoprovider.h \
    mplayerwindow.h \
    mediadata.h \
    audioequalizerlist.h \
    mediasettings.h \
    assstyles.h \
    filters.h \
    preferences.h \
    filesettingsbase.h \
    filesettings.h \
    filesettingshash.h \
    tvsettings.h \
    images.h \
    inforeader.h \
    deviceinfo.h \
    recents.h \
    urlhistory.h \
    core.h \
    logwindow.h \
    infofile.h \
    seekwidget.h \
    mytablewidget.h \
    shortcutgetter.h \
    actionseditor.h \
    filechooser.h \
    preferencesdialog.h \
    mycombobox.h \
    tristatecombo.h \
    languages.h \
    selectcolorbutton.h \
    prefwidget.h \
    prefgeneral.h \
    prefdrives.h \
    prefinterface.h \
    prefperformance.h \
    prefinput.h \
    prefsubtitles.h \
    prefadvanced.h \
    prefplaylist.h \
    preftv.h \
    filepropertiesdialog.h \
    playlist.h \
    playlistpreferences.h \
    playlistdock.h \
    verticaltext.h \
    eqslider.h \
    videoequalizer.h \
    audioequalizer.h \
    myslider.h \
    timeslider.h \
    inputdvddirectory.h \
    inputurl.h \
    myaction.h \
    myactiongroup.h \
    myserver.h \
    myclient.h \
    filedialog.h \
    inputmplayerversion.h \
    about.h \
    errordialog.h \
    timedialog.h \
    findsubtitles/simplehttp.h \
    findsubtitles/osparser.h \
    findsubtitles/findsubtitlesconfigdialog.h \
    findsubtitles/findsubtitleswindow.h \
    videopreview/videopreview.h \
    videopreview/videopreviewconfigdialog.h \
    favorites.h \
    tvlist.h \
    favoriteeditor.h \
    basegui.h \
    baseguiplus.h \
    floatingwidget.h \
    widgetactions.h \
    toolbareditor.h \
    defaultgui.h \
    minigui.h \
    mpcgui/mpcgui.h \
    mpcgui/mpcstyles.h \
    umplayer.h \
    clhelp.h \
    qpropertysetter.h \
    mediabarpanel.h \
    playcontrol.h \
    myicon.h \
    mybutton.h \
    panelseeker.h \
    volumecontrolpanel.h \
    youtubesearchbox.h \
    equalizer.h \
    synchronization.h \
    yttabbar.h \
    ytdelegate.h \
    ytdialog.h \
    ytdataapi.h \
    retrievevideourl.h \
    scdialog.h \
    scdelegate.h \
    scdataapi.h \
    updatenotice.h \
    downloadprogressdialog.h \
    recordingdialog.h \
    downloadfile.h \
    myborder.h \
    searchbox.h \
    faderwidget.h \
    languageselection.h \
    nonscrolltextview.h

SOURCES += mediapanel.cpp \
    version.cpp \
    global.cpp \
    paths.cpp \
    helper.cpp \
    colorutils.cpp \
    translator.cpp \
    subtracks.cpp \
    tracks.cpp \
    titletracks.cpp \
    discname.cpp \
    extensions.cpp \
    desktopinfo.cpp \
    myprocess.cpp \
    mplayerversion.cpp \
    mplayerprocess.cpp \
    infoprovider.cpp \
    mplayerwindow.cpp \
    mediadata.cpp \
    mediasettings.cpp \
    assstyles.cpp \
    filters.cpp \
    preferences.cpp \
    filesettingsbase.cpp \
    filesettings.cpp \
    filesettingshash.cpp \
    tvsettings.cpp \
    images.cpp \
    inforeader.cpp \
    deviceinfo.cpp \
    recents.cpp \
    urlhistory.cpp \
    core.cpp \
    logwindow.cpp \
    infofile.cpp \
    seekwidget.cpp \
    mytablewidget.cpp \
    shortcutgetter.cpp \
    actionseditor.cpp \
    filechooser.cpp \
    preferencesdialog.cpp \
    mycombobox.cpp \
    tristatecombo.cpp \
    languages.cpp \
    selectcolorbutton.cpp \
    prefwidget.cpp \
    prefgeneral.cpp \
    prefdrives.cpp \
    prefinterface.cpp \
    prefperformance.cpp \
    prefinput.cpp \
    prefsubtitles.cpp \
    prefadvanced.cpp \
    prefplaylist.cpp \
    preftv.cpp \
    filepropertiesdialog.cpp \
    playlist.cpp \
    playlistpreferences.cpp \
    playlistdock.cpp \
    verticaltext.cpp \
    eqslider.cpp \
    videoequalizer.cpp \
    audioequalizer.cpp \
    myslider.cpp \
    timeslider.cpp \
    inputdvddirectory.cpp \
    inputurl.cpp \
    myaction.cpp \
    myactiongroup.cpp \
    myserver.cpp \
    myclient.cpp \
    filedialog.cpp \
    inputmplayerversion.cpp \
    about.cpp \
    errordialog.cpp \
    timedialog.cpp \
    findsubtitles/simplehttp.cpp \
    findsubtitles/osparser.cpp \
    findsubtitles/findsubtitlesconfigdialog.cpp \
    findsubtitles/findsubtitleswindow.cpp \
    videopreview/videopreview.cpp \
    videopreview/videopreviewconfigdialog.cpp \
    favorites.cpp \
    tvlist.cpp \
    favoriteeditor.cpp \
    basegui.cpp \
    baseguiplus.cpp \
    floatingwidget.cpp \
    widgetactions.cpp \
    toolbareditor.cpp \
    defaultgui.cpp \
    minigui.cpp \
    mpcgui/mpcgui.cpp \
    mpcgui/mpcstyles.cpp \
    clhelp.cpp \
    umplayer.cpp \
    main.cpp \
    qpropertysetter.cpp \
    mediabarpanel.cpp \
    playcontrol.cpp \
    myicon.cpp \
    mybutton.cpp \
    panelseeker.cpp \
    volumecontrolpanel.cpp \
    youtubesearchbox.cpp \
    equalizer.cpp \
    synchronization.cpp \
    yttabbar.cpp \
    ytdelegate.cpp \
    ytdialog.cpp \
    ytdataapi.cpp \
    retrievevideourl.cpp \
    scdialog.cpp \
    scdelegate.cpp \
    scdataapi.cpp \
    updatenotice.cpp \
    downloadprogressdialog.cpp \
    recordingdialog.cpp \
    downloadfile.cpp \
    myborder.cpp \
    searchbox.cpp \
    faderwidget.cpp \
    languageselection.cpp \
    nonscrolltextview.cpp

# libqxt
contains(DEFINES, USE_QXT) { 
    CONFIG += qxt
    QXT += core
}
FORMS = mediapanel.ui \
    inputdvddirectory.ui \
    logwindowbase.ui \
    filepropertiesdialog.ui \
    eqslider.ui \
    seekwidget.ui \
    inputurl.ui \
    preferencesdialog.ui \
    prefgeneral.ui \
    prefdrives.ui \
    prefinterface.ui \
    prefperformance.ui \
    prefinput.ui \
    prefsubtitles.ui \
    prefadvanced.ui \
    prefplaylist.ui \
    preftv.ui \
    favoriteeditor.ui \
    about.ui \
    inputmplayerversion.ui \
    errordialog.ui \
    timedialog.ui \
    playlistpreferences.ui \
    filechooser.ui \
    findsubtitles/findsubtitleswindow.ui \
    findsubtitles/findsubtitlesconfigdialog.ui \
    videopreview/videopreviewconfigdialog.ui \
    mediabarpanel.ui \
    synchronization.ui \
    languageselection.ui

TRANSLATIONS = translations/umplayer_es.ts \
    translations/umplayer_de.ts \
    translations/umplayer_sk.ts \
    translations/umplayer_it.ts \
    translations/umplayer_fr.ts \
    translations/umplayer_zh_CN.ts \
    translations/umplayer_ru_RU.ts \
    translations/umplayer_hu.ts \
    translations/umplayer_en_US.ts \
    translations/umplayer_pl.ts \
    translations/umplayer_ja.ts \
    translations/umplayer_nl.ts \
    translations/umplayer_uk_UA.ts \
    translations/umplayer_pt_BR.ts \
    translations/umplayer_ka.ts \
    translations/umplayer_cs.ts \
    translations/umplayer_bg.ts \
    translations/umplayer_tr.ts \
    translations/umplayer_sv.ts \
    translations/umplayer_sr.ts \
    translations/umplayer_zh_TW.ts \
    translations/umplayer_ro_RO.ts \
    translations/umplayer_pt.ts \
    translations/umplayer_el_GR.ts \
    translations/umplayer_fi.ts \
    translations/umplayer_ko.ts \
    translations/umplayer_mk.ts \
    translations/umplayer_eu.ts \
    translations/umplayer_ca.ts \
    translations/umplayer_sl_SI.ts \
    translations/umplayer_ar_SY.ts \
    translations/umplayer_ku.ts \
    translations/umplayer_gl.ts \
    translations/umplayer_vi_VN.ts \
    translations/umplayer_et.ts
contains( DEFINES, DOWNLOAD_SUBS ) { 
    INCLUDEPATH += findsubtitles/filedownloader \
        findsubtitles/quazip
    DEPENDPATH += findsubtitles/filedownloader \
        findsubtitles/quazip
    HEADERS += filedownloader.h \
        subchooserdialog.h
    SOURCES += filedownloader.cpp \
        subchooserdialog.cpp
    FORMS += subchooserdialog.ui
    HEADERS += crypt.h \
        ioapi.h \
        quazip.h \
        quazipfile.h \
        quazipfileinfo.h \
        quazipnewinfo.h \
        unzip.h \
        zip.h
    SOURCES += ioapi.c \
        quazip.cpp \
        quazipfile.cpp \
        quazipnewinfo.cpp \
        unzip.c \
        zip.c
    LIBS += -lz
    win32 { 
        INCLUDEPATH += c:\\development\\zlib-1.2.3
        LIBS += -Lc:\\development\\zlib-1.2.3
    }
}
unix { 
    UI_DIR = .ui
    MOC_DIR = .moc
    OBJECTS_DIR = .obj
    DEFINES += DATA_PATH=$(DATA_PATH)
    DEFINES += DOC_PATH=$(DOC_PATH)
    DEFINES += TRANSLATION_PATH=$(TRANSLATION_PATH)
    DEFINES += THEMES_PATH=$(THEMES_PATH)
    DEFINES += SHORTCUTS_PATH=$(SHORTCUTS_PATH)
}

# DEFINES += NO_DEBUG_ON_CONSOLE
# DEFINES += KDE_SUPPORT
# INCLUDEPATH += /opt/kde3/include/
# LIBS += -lkio -L/opt/kde3/lib/
# contains( DEFINES, KDE_SUPPORT) {
# HEADERS += mysystemtrayicon.h
# SOURCES += mysystemtrayicon.cpp
# }
# HEADERS += prefassociations.h winfileassoc.h
# SOURCES += prefassociations.cpp winfileassoc.cpp
# FORMS += prefassociations.ui
win32 { 
    DEFINES += SCREENSAVER_OFF
    contains( DEFINES, SCREENSAVER_OFF ) { 
        HEADERS += screensaver.h
        SOURCES += screensaver.cpp
    }
    !contains( DEFINES, PORTABLE_APP ):DEFINES += USE_ASSOCIATIONS
    contains( DEFINES, USE_ASSOCIATIONS ) { 
        HEADERS += prefassociations.h \
            winfileassoc.h \
            fileassocquestion.h
        SOURCES += prefassociations.cpp \
            winfileassoc.cpp \
            fileassocquestion.cpp
        FORMS += prefassociations.ui \
            fileassocquestion.ui
    }
    contains(TEMPLATE,vcapp):LIBS += ole32.lib \
        user32.lib shell32.lib
    else:LIBS += libole32 libshell32
    RC_FILE = umplayer.rc
    DEFINES += NO_DEBUG_ON_CONSOLE
    HEADERS += win7_include.h AppUserModel.h
    SOURCES += AppUserModel.cpp
}

mac {
  HEADERS += mplayerconnection.h objc++bridge.h globaldataclass.h macstyle.h
  OBJECTIVE_SOURCES += mplayerconnection.mm objc++bridge.mm
  SOURCES += globaldataclass.cpp macstyle.cpp
  LIBS += -framework Cocoa -framework QuartzCore
  ICON = umplayer.icns
  QT += opengl
  CONFIG += x86 x86_64

}
