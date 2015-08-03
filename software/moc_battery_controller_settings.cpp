/****************************************************************************
** Meta object code from reading C++ file 'battery_controller_settings.h'
**
** Created: Tue Jul 28 05:44:21 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/battery_controller_settings.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'battery_controller_settings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BatteryControllerSettings[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       4,   24, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x05,
      47,   26,   26,   26, 0x05,

 // properties: name, type, flags
      72,   68, 0x02095001,
      91,   83, 0x0a095001,
      98,   83, 0x0a495103,
     109,   83, 0x0a495103,

 // properties: notify_signal_id
       0,
       0,
       0,
       1,

       0        // eod
};

static const char qt_meta_stringdata_BatteryControllerSettings[] = {
    "BatteryControllerSettings\0\0"
    "customNameChanged()\0serviceTypeChanged()\0"
    "int\0deviceType\0QString\0serial\0customName\0"
    "serviceType\0"
};

void BatteryControllerSettings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BatteryControllerSettings *_t = static_cast<BatteryControllerSettings *>(_o);
        switch (_id) {
        case 0: _t->customNameChanged(); break;
        case 1: _t->serviceTypeChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData BatteryControllerSettings::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BatteryControllerSettings::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BatteryControllerSettings,
      qt_meta_data_BatteryControllerSettings, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BatteryControllerSettings::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BatteryControllerSettings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BatteryControllerSettings::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BatteryControllerSettings))
        return static_cast<void*>(const_cast< BatteryControllerSettings*>(this));
    return QObject::qt_metacast(_clname);
}

int BatteryControllerSettings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = deviceType(); break;
        case 1: *reinterpret_cast< QString*>(_v) = serial(); break;
        case 2: *reinterpret_cast< QString*>(_v) = customName(); break;
        case 3: *reinterpret_cast< QString*>(_v) = serviceType(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 2: setCustomName(*reinterpret_cast< QString*>(_v)); break;
        case 3: setServiceType(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void BatteryControllerSettings::customNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void BatteryControllerSettings::serviceTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
