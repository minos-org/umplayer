/****************************************************************************
** Meta object code from reading C++ file 'filechooser.h'
**
** Created: Mon 8. Feb 19:27:43 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../filechooser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filechooser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FileChooser[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       5,   49, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,   39,   12,   12, 0x0a,
      69,   61,   12,   12, 0x0a,
      96,   89,   12,   12, 0x0a,
     120,  115,   12,   12, 0x0a,
     154,  146,   12,   12, 0x0a,
     187,   12,   12,   12, 0x09,

 // properties: name, type, flags
      39,  207, 0x0a095103,
      61,  207, 0x0a095103,
      89,  207, 0x0a095103,
     226,  215, 0x0009510b,
     146,  237, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_FileChooser[] = {
    "FileChooser\0\0file\0fileChanged(QString)\0"
    "text\0setText(QString)\0caption\0"
    "setCaption(QString)\0filter\0"
    "setFilter(QString)\0type\0"
    "setDialogType(DialogType)\0options\0"
    "setOptions(QFileDialog::Options)\0"
    "on_button_clicked()\0QString\0DialogType\0"
    "dialogType\0QFileDialog::Options\0"
};

#ifdef Q_NO_DATA_RELOCATION
static const QMetaObjectAccessor qt_meta_extradata_FileChooser[] = {
        QFileDialog::getStaticMetaObject,
#else
static const QMetaObject *qt_meta_extradata_FileChooser[] = {
        &QFileDialog::staticMetaObject,
#endif //Q_NO_DATA_RELOCATION
    0
};

static const QMetaObjectExtraData qt_meta_extradata2_FileChooser = {
    qt_meta_extradata_FileChooser, 0 
};

const QMetaObject FileChooser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FileChooser,
      qt_meta_data_FileChooser, &qt_meta_extradata2_FileChooser }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FileChooser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FileChooser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FileChooser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FileChooser))
        return static_cast<void*>(const_cast< FileChooser*>(this));
    if (!strcmp(_clname, "Ui::FileChooser"))
        return static_cast< Ui::FileChooser*>(const_cast< FileChooser*>(this));
    return QWidget::qt_metacast(_clname);
}

int FileChooser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fileChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: setText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: setCaption((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: setFilter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: setDialogType((*reinterpret_cast< DialogType(*)>(_a[1]))); break;
        case 5: setOptions((*reinterpret_cast< QFileDialog::Options(*)>(_a[1]))); break;
        case 6: on_button_clicked(); break;
        default: ;
        }
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        case 1: *reinterpret_cast< QString*>(_v) = caption(); break;
        case 2: *reinterpret_cast< QString*>(_v) = filter(); break;
        case 3: *reinterpret_cast< DialogType*>(_v) = dialogType(); break;
        case 4: *reinterpret_cast< QFileDialog::Options*>(_v) = options(); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setText(*reinterpret_cast< QString*>(_v)); break;
        case 1: setCaption(*reinterpret_cast< QString*>(_v)); break;
        case 2: setFilter(*reinterpret_cast< QString*>(_v)); break;
        case 3: setDialogType(*reinterpret_cast< DialogType*>(_v)); break;
        case 4: setOptions(*reinterpret_cast< QFileDialog::Options*>(_v)); break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void FileChooser::fileChanged(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
