/****************************************************************************
** Meta object code from reading C++ file 'communication.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../communication.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'communication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Communication[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      62,   14,   14,   14, 0x08,
      72,   14,   14,   14, 0x08,
      91,   14,   14,   14, 0x08,
     113,   14,   14,   14, 0x08,
     130,   14,   14,   14, 0x08,
     145,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Communication[] = {
    "Communication\0\0port,priv\0"
    "statisticsUpdate(Communication*,int)\0"
    "restart()\0clientConnection()\0"
    "clientDisconnection()\0clientReadData()\0"
    "portReadData()\0statistics()\0"
};

void Communication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Communication *_t = static_cast<Communication *>(_o);
        switch (_id) {
        case 0: _t->statisticsUpdate((*reinterpret_cast< Communication*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->restart(); break;
        case 2: _t->clientConnection(); break;
        case 3: _t->clientDisconnection(); break;
        case 4: _t->clientReadData(); break;
        case 5: _t->portReadData(); break;
        case 6: _t->statistics(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Communication::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Communication::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Communication,
      qt_meta_data_Communication, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Communication::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Communication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Communication::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Communication))
        return static_cast<void*>(const_cast< Communication*>(this));
    return QObject::qt_metacast(_clname);
}

int Communication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Communication::statisticsUpdate(Communication * _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
