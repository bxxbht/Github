/****************************************************************************
** Meta object code from reading C++ file 'alertbox.h'
**
** Created: Tue Apr 10 13:55:43 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../alertbox.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'alertbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AlertBox[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x08,
      20,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AlertBox[] = {
    "AlertBox\0\0refresh()\0blanking()\0"
};

const QMetaObject AlertBox::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_AlertBox,
      qt_meta_data_AlertBox, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AlertBox::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AlertBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AlertBox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AlertBox))
        return static_cast<void*>(const_cast< AlertBox*>(this));
    return QLabel::qt_metacast(_clname);
}

int AlertBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refresh(); break;
        case 1: blanking(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
