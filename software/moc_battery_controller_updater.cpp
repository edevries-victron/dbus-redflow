/****************************************************************************
** Meta object code from reading C++ file 'battery_controller_updater.h'
**
** Created: Tue Jul 28 05:52:05 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/battery_controller_updater.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'battery_controller_updater.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BatteryControllerUpdater[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
      83,   58,   25,   25, 0x08,
     139,  115,   25,   25, 0x08,
     210,  182,   25,   25, 0x08,
     255,   25,   25,   25, 0x08,
     272,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BatteryControllerUpdater[] = {
    "BatteryControllerUpdater\0\0"
    "infoChanged(BatteryController*)\0"
    "errorType,addr,exception\0"
    "onErrorReceived(int,quint8,int)\0"
    "function,addr,registers\0"
    "onReadCompleted(int,quint8,QList<quint16>)\0"
    "function,addr,address,value\0"
    "onWriteCompleted(int,quint8,quint16,quint16)\0"
    "onWaitFinished()\0onUpdateSettings()\0"
};

void BatteryControllerUpdater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BatteryControllerUpdater *_t = static_cast<BatteryControllerUpdater *>(_o);
        switch (_id) {
        case 0: _t->infoChanged((*reinterpret_cast< BatteryController*(*)>(_a[1]))); break;
        case 1: _t->onErrorReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->onReadCompleted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< const QList<quint16>(*)>(_a[3]))); break;
        case 3: _t->onWriteCompleted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< quint8(*)>(_a[2])),(*reinterpret_cast< quint16(*)>(_a[3])),(*reinterpret_cast< quint16(*)>(_a[4]))); break;
        case 4: _t->onWaitFinished(); break;
        case 5: _t->onUpdateSettings(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BatteryControllerUpdater::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BatteryControllerUpdater::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BatteryControllerUpdater,
      qt_meta_data_BatteryControllerUpdater, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BatteryControllerUpdater::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BatteryControllerUpdater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BatteryControllerUpdater::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BatteryControllerUpdater))
        return static_cast<void*>(const_cast< BatteryControllerUpdater*>(this));
    return QObject::qt_metacast(_clname);
}

int BatteryControllerUpdater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void BatteryControllerUpdater::infoChanged(BatteryController * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
