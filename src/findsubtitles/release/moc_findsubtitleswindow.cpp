/****************************************************************************
** Meta object code from reading C++ file 'findsubtitleswindow.h'
**
** Created: Mon 8. Feb 19:27:54 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../findsubtitleswindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'findsubtitleswindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FindSubtitlesWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      30,   21,   20,   20, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,   21,   20,   20, 0x0a,
      81,   76,   20,   20, 0x0a,
     102,   20,   20,   20, 0x0a,
     112,   20,   20,   20, 0x0a,
     123,   20,   20,   20, 0x0a,
     136,  134,   20,   20, 0x0a,
     174,  167,   20,   20, 0x09,
     195,   20,   20,   20, 0x09,
     222,  216,   20,   20, 0x09,
     246,  241,   20,   20, 0x09,
     277,  266,   20,   20, 0x09,
     309,   20,   20,   20, 0x09,
     328,   20,   20,   20, 0x09,
     359,  350,   20,   20, 0x09,
     387,  381,   20,   20, 0x09,
     431,  414,   20,   20, 0x09,
     479,  475,   20,   20, 0x09,
     510,  503,   20,   20, 0x09,
     540,   20,   20,   20, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_FindSubtitlesWindow[] = {
    "FindSubtitlesWindow\0\0filename\0"
    "subtitleDownloaded(QString)\0"
    "setMovie(QString)\0lang\0setLanguage(QString)\0"
    "refresh()\0download()\0copyLink()\0b\0"
    "setIncludeLangOnFilename(bool)\0filter\0"
    "applyFilter(QString)\0applyCurrentFilter()\0"
    "error\0showError(QString)\0host\0"
    "connecting(QString)\0done,total\0"
    "updateDataReadProgress(int,int)\0"
    "downloadFinished()\0updateRefreshButton()\0"
    "xml_text\0parseInfo(QByteArray)\0index\0"
    "itemActivated(QModelIndex)\0current,previous\0"
    "currentItemChanged(QModelIndex,QModelIndex)\0"
    "pos\0showContextMenu(QPoint)\0buffer\0"
    "archiveDownloaded(QByteArray)\0"
    "on_configure_button_clicked()\0"
};

const QMetaObject FindSubtitlesWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FindSubtitlesWindow,
      qt_meta_data_FindSubtitlesWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FindSubtitlesWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FindSubtitlesWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FindSubtitlesWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FindSubtitlesWindow))
        return static_cast<void*>(const_cast< FindSubtitlesWindow*>(this));
    if (!strcmp(_clname, "Ui::FindSubtitlesWindow"))
        return static_cast< Ui::FindSubtitlesWindow*>(const_cast< FindSubtitlesWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int FindSubtitlesWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: subtitleDownloaded((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: setMovie((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: setLanguage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: refresh(); break;
        case 4: download(); break;
        case 5: copyLink(); break;
        case 6: setIncludeLangOnFilename((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: applyFilter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: applyCurrentFilter(); break;
        case 9: showError((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: connecting((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: updateDataReadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: downloadFinished(); break;
        case 13: updateRefreshButton(); break;
        case 14: parseInfo((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 15: itemActivated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 16: currentItemChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 17: showContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 18: archiveDownloaded((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 19: on_configure_button_clicked(); break;
        default: ;
        }
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void FindSubtitlesWindow::subtitleDownloaded(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
