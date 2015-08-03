/****************************************************************************
** Meta object code from reading C++ file 'battery_controller_bridge.h'
**
** Created: Fri Jul 31 06:01:28 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/battery_controller_bridge.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'battery_controller_bridge.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BatteryControllerBridge[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      33,   25,   24,   24, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_BatteryControllerBridge[] = {
    "BatteryControllerBridge\0\0bc,path\0"
    "produceBatteryInfo(BatteryController*,QString)\0"
};

void BatteryControllerBridge::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BatteryControllerBridge *_t = static_cast<BatteryControllerBridge *>(_o);
        switch (_id) {
        case 0: _t->produceBatteryInfo((*reinterpret_cast< BatteryController*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BatteryControllerBridge::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BatteryControllerBridge::staticMetaObject = {
    { &DBusBridge::staticMetaObject, qt_meta_stringdata_BatteryControllerBridge,
      qt_meta_data_BatteryControllerBridge, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BatteryControllerBridge::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BatteryControllerBridge::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BatteryControllerBridge::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BatteryControllerBridge))
        return static_cast<void*>(const_cast< BatteryControllerBridge*>(this));
    return DBusBridge::qt_metacast(_clname);
}

int BatteryControllerBridge::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DBusBridge::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
