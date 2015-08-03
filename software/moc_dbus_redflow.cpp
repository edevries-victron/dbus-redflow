/****************************************************************************
** Meta object code from reading C++ file 'dbus_redflow.h'
**
** Created: Tue Jul 28 05:56:46 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/dbus_redflow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dbus_redflow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DBusRedflow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      30,   12,   12,   12, 0x08,
      46,   12,   12,   12, 0x08,
      76,   12,   12,   12, 0x08,
      98,   12,   12,   12, 0x08,
     117,   12,   12,   12, 0x08,
     156,  144,   12,   12, 0x08,
     183,   12,   12,   12, 0x08,
     203,   12,   12,   12, 0x08,
     226,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DBusRedflow[] = {
    "DBusRedflow\0\0connectionLost()\0"
    "onDeviceFound()\0onDeviceSettingsInitialized()\0"
    "onDeviceInitialized()\0onConnectionLost()\0"
    "onConnectionStateChanged()\0description\0"
    "onSerialEvent(const char*)\0"
    "onServicesChanged()\0onServiceTypeChanged()\0"
    "onControlLoopEnabledChanged()\0"
};

void DBusRedflow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DBusRedflow *_t = static_cast<DBusRedflow *>(_o);
        switch (_id) {
        case 0: _t->connectionLost(); break;
        case 1: _t->onDeviceFound(); break;
        case 2: _t->onDeviceSettingsInitialized(); break;
        case 3: _t->onDeviceInitialized(); break;
        case 4: _t->onConnectionLost(); break;
        case 5: _t->onConnectionStateChanged(); break;
        case 6: _t->onSerialEvent((*reinterpret_cast< const char*(*)>(_a[1]))); break;
        case 7: _t->onServicesChanged(); break;
        case 8: _t->onServiceTypeChanged(); break;
        case 9: _t->onControlLoopEnabledChanged(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DBusRedflow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DBusRedflow::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DBusRedflow,
      qt_meta_data_DBusRedflow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DBusRedflow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DBusRedflow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DBusRedflow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DBusRedflow))
        return static_cast<void*>(const_cast< DBusRedflow*>(this));
    return QObject::qt_metacast(_clname);
}

int DBusRedflow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void DBusRedflow::connectionLost()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
