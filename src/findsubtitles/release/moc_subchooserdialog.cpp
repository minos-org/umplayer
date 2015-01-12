/****************************************************************************
** Meta object code from reading C++ file 'subchooserdialog.h'
**
** Created: Mon 8. Feb 22:56:20 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../subchooserdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'subchooserdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SubChooserDialog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x09,
      41,   17,   17,   17, 0x09,
      70,   65,   17,   17, 0x09,
     104,   65,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SubChooserDialog[] = {
    "SubChooserDialog\0\0selectAllClicked(bool)\0"
    "selectNoneClicked(bool)\0item\0"
    "listItemClicked(QListWidgetItem*)\0"
    "listItemPressed(QListWidgetItem*)\0"
};

const QMetaObject SubChooserDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SubChooserDialog,
      qt_meta_data_SubChooserDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SubChooserDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SubChooserDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SubChooserDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SubChooserDialog))
        return static_cast<void*>(const_cast< SubChooserDialog*>(this));
    if (!strcmp(_clname, "Ui::SubChooserDialog"))
        return static_cast< Ui::SubChooserDialog*>(const_cast< SubChooserDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int SubChooserDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: selectAllClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: selectNoneClicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: listItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: listItemPressed((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
