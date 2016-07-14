/****************************************************************************
** Meta object code from reading C++ file 'partition.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../partition.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'partition.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Partition[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       3,   29, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,
      41,   34,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      60,   10,   10,   10, 0x0a,

 // enums: name, flags, count, data
      70, 0x0,    2,   41,
      77, 0x0,    4,   45,
      83, 0x0,    3,   53,

 // enum data: key, value
      88, uint(Partition::OK),
      91, uint(Partition::Failed),
      98, uint(Partition::Malformed),
     108, uint(Partition::Incomplted),
     119, uint(Partition::FileNotExist),
     132, uint(Partition::CrcError),
     141, uint(Partition::MMCRaw),
     148, uint(Partition::File),
     153, uint(Partition::Serial),

       0        // eod
};

static const char qt_meta_stringdata_Partition[] = {
    "Partition\0\0value\0setProgress(int)\0"
    "status\0setStatus(QString)\0timeout()\0"
    "Status\0Error\0Type\0OK\0Failed\0Malformed\0"
    "Incomplted\0FileNotExist\0CrcError\0"
    "MMCRaw\0File\0Serial\0"
};

void Partition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Partition *_t = static_cast<Partition *>(_o);
        switch (_id) {
        case 0: _t->setProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setStatus((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->timeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Partition::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Partition::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Partition,
      qt_meta_data_Partition, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Partition::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Partition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Partition::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Partition))
        return static_cast<void*>(const_cast< Partition*>(this));
    return QThread::qt_metacast(_clname);
}

int Partition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Partition::setProgress(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Partition::setStatus(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
