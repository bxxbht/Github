/****************************************************************************
** Meta object code from reading C++ file 'cardiotachometer.h'
**
** Created: Tue Apr 10 13:55:44 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../cardiotachometer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cardiotachometer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_HeartBeat[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HeartBeat[] = {
    "HeartBeat\0\0heartBeat()\0"
};

const QMetaObject HeartBeat::staticMetaObject = {
    { &IconWidget::staticMetaObject, qt_meta_stringdata_HeartBeat,
      qt_meta_data_HeartBeat, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &HeartBeat::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *HeartBeat::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *HeartBeat::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HeartBeat))
        return static_cast<void*>(const_cast< HeartBeat*>(this));
    return IconWidget::qt_metacast(_clname);
}

int HeartBeat::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = IconWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: heartBeat(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_Cardiotachometer[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_Cardiotachometer[] = {
    "Cardiotachometer\0"
};

const QMetaObject Cardiotachometer::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_Cardiotachometer,
      qt_meta_data_Cardiotachometer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Cardiotachometer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Cardiotachometer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Cardiotachometer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Cardiotachometer))
        return static_cast<void*>(const_cast< Cardiotachometer*>(this));
    return QFrame::qt_metacast(_clname);
}

int Cardiotachometer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
