/****************************************************************************
** Meta object code from reading C++ file 'v_busitem_private.h'
**
** Created: Mon Jul 27 00:29:33 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ext/velib/src/qt/v_busitem_private.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'v_busitem_private.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VBusItemPrivate[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      51,   48,   16,   16, 0x0a,
     109,   16,   96,   16, 0x0a,
     128,   16,  120,   16, 0x0a,
     148,  142,  138,   16, 0x0a,
     187,  171,  120,   16, 0x0a,
     215,   16,   16,   16, 0x0a,
     255,   16,   16,   16, 0x0a,
     294,   16,   16,   16, 0x0a,
     332,   16,   16,   16, 0x0a,
     370,   16,   16,   16, 0x0a,
     416,   16,   16,   16, 0x0a,
     455,   16,   16,   16, 0x0a,
     497,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VBusItemPrivate[] = {
    "VBusItemPrivate\0\0PropertiesChanged(QVariantMap)\0"
    ",,\0serviceOwnerChanged(QString,QString,QString)\0"
    "QDBusVariant\0GetValue()\0QString\0"
    "GetText()\0int\0value\0SetValue(QDBusVariant)\0"
    "language,length\0GetDescription(QString,int)\0"
    "valueObtained(QDBusPendingCallWatcher*)\0"
    "textObtained(QDBusPendingCallWatcher*)\0"
    "minObtained(QDBusPendingCallWatcher*)\0"
    "maxObtained(QDBusPendingCallWatcher*)\0"
    "descriptionObtained(QDBusPendingCallWatcher*)\0"
    "setValueDone(QDBusPendingCallWatcher*)\0"
    "defaultObtained(QDBusPendingCallWatcher*)\0"
    "setDefaultDone(QDBusPendingCallWatcher*)\0"
};

void VBusItemPrivate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VBusItemPrivate *_t = static_cast<VBusItemPrivate *>(_o);
        switch (_id) {
        case 0: _t->PropertiesChanged((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 1: _t->serviceOwnerChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 2: { QDBusVariant _r = _t->GetValue();
            if (_a[0]) *reinterpret_cast< QDBusVariant*>(_a[0]) = _r; }  break;
        case 3: { QString _r = _t->GetText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 4: { int _r = _t->SetValue((*reinterpret_cast< const QDBusVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { QString _r = _t->GetDescription((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 6: _t->valueObtained((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        case 7: _t->textObtained((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        case 8: _t->minObtained((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        case 9: _t->maxObtained((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        case 10: _t->descriptionObtained((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        case 11: _t->setValueDone((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        case 12: _t->defaultObtained((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        case 13: _t->setDefaultDone((*reinterpret_cast< QDBusPendingCallWatcher*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VBusItemPrivate::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VBusItemPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VBusItemPrivate,
      qt_meta_data_VBusItemPrivate, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VBusItemPrivate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VBusItemPrivate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VBusItemPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VBusItemPrivate))
        return static_cast<void*>(const_cast< VBusItemPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int VBusItemPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
