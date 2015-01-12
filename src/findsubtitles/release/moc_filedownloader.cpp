/****************************************************************************
** Meta object code from reading C++ file 'filedownloader.h'
**
** Created: Mon 8. Feb 22:56:18 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../filedownloader/filedownloader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filedownloader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FileDownloader[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   16,   15,   15, 0x05,
      59,   52,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      87,   83,   15,   15, 0x0a,
     102,   15,   15,   15, 0x0a,
     136,  119,   15,   15, 0x08,
     181,  166,   15,   15, 0x08,
     244,  221,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FileDownloader[] = {
    "FileDownloader\0\0buffer\0"
    "downloadFinished(QByteArray)\0reason\0"
    "downloadFailed(QString)\0url\0download(QUrl)\0"
    "cancelDownload()\0request_id,error\0"
    "httpRequestFinished(int,bool)\0"
    "responseHeader\0readResponseHeader(QHttpResponseHeader)\0"
    "bytes_read,total_bytes\0"
    "updateDataReadProgress(int,int)\0"
};

const QMetaObject FileDownloader::staticMetaObject = {
    { &QProgressDialog::staticMetaObject, qt_meta_stringdata_FileDownloader,
      qt_meta_data_FileDownloader, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FileDownloader::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FileDownloader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FileDownloader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FileDownloader))
        return static_cast<void*>(const_cast< FileDownloader*>(this));
    return QProgressDialog::qt_metacast(_clname);
}

int FileDownloader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QProgressDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: downloadFinished((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 1: downloadFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: download((*reinterpret_cast< QUrl(*)>(_a[1]))); break;
        case 3: cancelDownload(); break;
        case 4: httpRequestFinished((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: readResponseHeader((*reinterpret_cast< const QHttpResponseHeader(*)>(_a[1]))); break;
        case 6: updateDataReadProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void FileDownloader::downloadFinished(const QByteArray & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FileDownloader::downloadFailed(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
