/****************************************************************************
** Meta object code from reading C++ file 'batteryController.h'
**
** Created: Mon Aug 3 00:32:54 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/batteryController.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'batteryController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BatteryController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
      13,   84, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      37,   18,   18,   18, 0x05,
      56,   18,   18,   18, 0x05,
      74,   18,   18,   18, 0x05,
      92,   18,   18,   18, 0x05,
     111,   18,   18,   18, 0x05,
     128,   18,   18,   18, 0x05,
     141,   18,   18,   18, 0x05,
     160,   18,   18,   18, 0x05,
     185,   18,   18,   18, 0x05,
     205,   18,   18,   18, 0x05,
     228,   18,   18,   18, 0x05,
     244,   18,   18,   18, 0x05,
     269,   18,   18,   18, 0x05,

 // properties: name, type, flags
     295,  288, 0x06495103,
     308,  304, 0x02495103,
     318,  288, 0x06495103,
     327,  304, 0x02495103,
     337,  304, 0x02495103,
     357,  341, 0x0049510b,
     373,  304, 0x02495103,
     384,  304, 0x02495103,
     406,  398, 0x0a095001,
     418,  398, 0x0a495103,
     425,  304, 0x02495103,
     441,  304, 0x02495103,
     451,  398, 0x0a095001,

 // properties: notify_signal_id
       0,
       1,
       2,
       7,
       6,
       8,
       9,
      10,
       0,
      11,
      12,
      13,
       0,

       0        // eod
};

static const char qt_meta_stringdata_BatteryController[] = {
    "BatteryController\0\0battAmpsChanged()\0"
    "battVoltsChanged()\0battTempChanged()\0"
    "bussAmpsChanged()\0bussVoltsChanged()\0"
    "airTempChanged()\0socChanged()\0"
    "battPowerChanged()\0connectionStateChanged()\0"
    "deviceTypeChanged()\0deviceSubTypeChanged()\0"
    "serialChanged()\0firmwareVersionChanged()\0"
    "errorCodeChanged()\0double\0BattAmps\0"
    "int\0BattVolts\0BattTemp\0BattPower\0SOC\0"
    "ConnectionState\0connectionState\0"
    "deviceType\0deviceSubType\0QString\0"
    "productName\0serial\0firmwareVersion\0"
    "errorCode\0portName\0"
};

void BatteryController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BatteryController *_t = static_cast<BatteryController *>(_o);
        switch (_id) {
        case 0: _t->battAmpsChanged(); break;
        case 1: _t->battVoltsChanged(); break;
        case 2: _t->battTempChanged(); break;
        case 3: _t->bussAmpsChanged(); break;
        case 4: _t->bussVoltsChanged(); break;
        case 5: _t->airTempChanged(); break;
        case 6: _t->socChanged(); break;
        case 7: _t->battPowerChanged(); break;
        case 8: _t->connectionStateChanged(); break;
        case 9: _t->deviceTypeChanged(); break;
        case 10: _t->deviceSubTypeChanged(); break;
        case 11: _t->serialChanged(); break;
        case 12: _t->firmwareVersionChanged(); break;
        case 13: _t->errorCodeChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData BatteryController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BatteryController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BatteryController,
      qt_meta_data_BatteryController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BatteryController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BatteryController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BatteryController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BatteryController))
        return static_cast<void*>(const_cast< BatteryController*>(this));
    return QObject::qt_metacast(_clname);
}

int BatteryController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = BattAmps(); break;
        case 1: *reinterpret_cast< int*>(_v) = BattVolts(); break;
        case 2: *reinterpret_cast< double*>(_v) = BattTemp(); break;
        case 3: *reinterpret_cast< int*>(_v) = BattPower(); break;
        case 4: *reinterpret_cast< int*>(_v) = SOC(); break;
        case 5: *reinterpret_cast< ConnectionState*>(_v) = connectionState(); break;
        case 6: *reinterpret_cast< int*>(_v) = deviceType(); break;
        case 7: *reinterpret_cast< int*>(_v) = deviceSubType(); break;
        case 8: *reinterpret_cast< QString*>(_v) = productName(); break;
        case 9: *reinterpret_cast< QString*>(_v) = serial(); break;
        case 10: *reinterpret_cast< int*>(_v) = firmwareVersion(); break;
        case 11: *reinterpret_cast< int*>(_v) = errorCode(); break;
        case 12: *reinterpret_cast< QString*>(_v) = portName(); break;
        }
        _id -= 13;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBattAmps(*reinterpret_cast< double*>(_v)); break;
        case 1: setBattVolts(*reinterpret_cast< int*>(_v)); break;
        case 2: setBattTemp(*reinterpret_cast< double*>(_v)); break;
        case 3: setBattPower(*reinterpret_cast< int*>(_v)); break;
        case 4: setSOC(*reinterpret_cast< int*>(_v)); break;
        case 5: setConnectionState(*reinterpret_cast< ConnectionState*>(_v)); break;
        case 6: setDeviceType(*reinterpret_cast< int*>(_v)); break;
        case 7: setDeviceSubType(*reinterpret_cast< int*>(_v)); break;
        case 9: setSerial(*reinterpret_cast< QString*>(_v)); break;
        case 10: setFirmwareVersion(*reinterpret_cast< int*>(_v)); break;
        case 11: setErrorCode(*reinterpret_cast< int*>(_v)); break;
        }
        _id -= 13;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 13;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void BatteryController::battAmpsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void BatteryController::battVoltsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void BatteryController::battTempChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void BatteryController::bussAmpsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void BatteryController::bussVoltsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void BatteryController::airTempChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void BatteryController::socChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void BatteryController::battPowerChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void BatteryController::connectionStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void BatteryController::deviceTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void BatteryController::deviceSubTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void BatteryController::serialChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void BatteryController::firmwareVersionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void BatteryController::errorCodeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}
QT_END_MOC_NAMESPACE
