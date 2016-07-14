/****************************************************************************
** Meta object code from reading C++ file 'partition.h'
**
** Created: Mon Jul 4 15:42:35 2016
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../partition.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'partition.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Partition[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       2,   24, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,
      45,   38,   10,   10, 0x05,

 // enums: name, flags, count, data
      68, 0x0,    2,   32,
      75, 0x0,    4,   36,

 // enum data: key, value
      81, uint(Partition::Success),
      89, uint(Partition::Failed),
      96, uint(Partition::Malformed),
     106, uint(Partition::Incomplted),
     117, uint(Partition::FileNotExist),
     130, uint(Partition::CrcError),

       0        // eod
};

static const char qt_meta_stringdata_Partition[] = {
    "Partition\0\0value\0progressChanged(int)\0"
    "status\0statusChanged(QString)\0Status\0"
    "Error\0Success\0Failed\0Malformed\0"
    "Incomplted\0FileNotExist\0CrcError\0"
};

void Partition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Partition *_t = static_cast<Partition *>(_o);
        switch (_id) {
        case 0: _t->progressChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->statusChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
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
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Partition::progressChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Partition::statusChanged(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_DummyPartition[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_DummyPartition[] = {
    "DummyPartition\0"
};

void DummyPartition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData DummyPartition::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DummyPartition::staticMetaObject = {
    { &Partition::staticMetaObject, qt_meta_stringdata_DummyPartition,
      qt_meta_data_DummyPartition, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DummyPartition::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DummyPartition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DummyPartition::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DummyPartition))
        return static_cast<void*>(const_cast< DummyPartition*>(this));
    return Partition::qt_metacast(_clname);
}

int DummyPartition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Partition::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_MmcPartition[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_MmcPartition[] = {
    "MmcPartition\0"
};

void MmcPartition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MmcPartition::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MmcPartition::staticMetaObject = {
    { &Partition::staticMetaObject, qt_meta_stringdata_MmcPartition,
      qt_meta_data_MmcPartition, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MmcPartition::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MmcPartition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MmcPartition::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MmcPartition))
        return static_cast<void*>(const_cast< MmcPartition*>(this));
    return Partition::qt_metacast(_clname);
}

int MmcPartition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Partition::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Ext2Partition[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_Ext2Partition[] = {
    "Ext2Partition\0"
};

void Ext2Partition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Ext2Partition::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Ext2Partition::staticMetaObject = {
    { &Partition::staticMetaObject, qt_meta_stringdata_Ext2Partition,
      qt_meta_data_Ext2Partition, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Ext2Partition::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Ext2Partition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Ext2Partition::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Ext2Partition))
        return static_cast<void*>(const_cast< Ext2Partition*>(this));
    return Partition::qt_metacast(_clname);
}

int Ext2Partition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Partition::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_IspPartition[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_IspPartition[] = {
    "IspPartition\0"
};

void IspPartition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData IspPartition::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject IspPartition::staticMetaObject = {
    { &Partition::staticMetaObject, qt_meta_stringdata_IspPartition,
      qt_meta_data_IspPartition, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &IspPartition::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *IspPartition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *IspPartition::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_IspPartition))
        return static_cast<void*>(const_cast< IspPartition*>(this));
    return Partition::qt_metacast(_clname);
}

int IspPartition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Partition::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_IapPartition[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_IapPartition[] = {
    "IapPartition\0"
};

void IapPartition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData IapPartition::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject IapPartition::staticMetaObject = {
    { &Partition::staticMetaObject, qt_meta_stringdata_IapPartition,
      qt_meta_data_IapPartition, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &IapPartition::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *IapPartition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *IapPartition::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_IapPartition))
        return static_cast<void*>(const_cast< IapPartition*>(this));
    return Partition::qt_metacast(_clname);
}

int IapPartition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Partition::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_FetalIapPartition[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_FetalIapPartition[] = {
    "FetalIapPartition\0"
};

void FetalIapPartition::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FetalIapPartition::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FetalIapPartition::staticMetaObject = {
    { &Partition::staticMetaObject, qt_meta_stringdata_FetalIapPartition,
      qt_meta_data_FetalIapPartition, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FetalIapPartition::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FetalIapPartition::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FetalIapPartition::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FetalIapPartition))
        return static_cast<void*>(const_cast< FetalIapPartition*>(this));
    return Partition::qt_metacast(_clname);
}

int FetalIapPartition::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Partition::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
