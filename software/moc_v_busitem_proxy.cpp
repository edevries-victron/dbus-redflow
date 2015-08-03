/****************************************************************************
** Meta object code from reading C++ file 'v_busitem_proxy.h'
**
** Created: Tue Jul 28 05:00:49 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ext/velib/src/qt/v_busitem_proxy.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'v_busitem_proxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VBusItemProxy[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       2,   59, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      86,   14,   54,   14, 0x0a,
     142,  126,   99,   14, 0x0a,
     170,   14,   54,   14, 0x0a,
     179,   14,   54,   14, 0x0a,
     188,   14,   99,   14, 0x0a,
     198,   14,   54,   14, 0x0a,
     232,   14,  209,   14, 0x0a,
     251,  245,  209,   14, 0x0a,

 // properties: name, type, flags
     282,  274, 0x0a095001,
     300,  287, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_VBusItemProxy[] = {
    "VBusItemProxy\0\0changes\0"
    "PropertiesChanged(QVariantMap)\0"
    "QDBusPendingReply<QDBusVariant>\0"
    "GetDefault()\0QDBusPendingReply<QString>\0"
    "language,length\0GetDescription(QString,int)\0"
    "GetMax()\0GetMin()\0GetText()\0GetValue()\0"
    "QDBusPendingReply<int>\0SetDefault()\0"
    "value\0SetValue(QDBusVariant)\0QString\0"
    "Text\0QDBusVariant\0Value\0"
};

void VBusItemProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VBusItemProxy *_t = static_cast<VBusItemProxy *>(_o);
        switch (_id) {
        case 0: _t->PropertiesChanged((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 1: { QDBusPendingReply<QDBusVariant> _r = _t->GetDefault();
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<QDBusVariant>*>(_a[0]) = _r; }  break;
        case 2: { QDBusPendingReply<QString> _r = _t->GetDescription((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<QString>*>(_a[0]) = _r; }  break;
        case 3: { QDBusPendingReply<QDBusVariant> _r = _t->GetMax();
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<QDBusVariant>*>(_a[0]) = _r; }  break;
        case 4: { QDBusPendingReply<QDBusVariant> _r = _t->GetMin();
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<QDBusVariant>*>(_a[0]) = _r; }  break;
        case 5: { QDBusPendingReply<QString> _r = _t->GetText();
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<QString>*>(_a[0]) = _r; }  break;
        case 6: { QDBusPendingReply<QDBusVariant> _r = _t->GetValue();
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<QDBusVariant>*>(_a[0]) = _r; }  break;
        case 7: { QDBusPendingReply<int> _r = _t->SetDefault();
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<int>*>(_a[0]) = _r; }  break;
        case 8: { QDBusPendingReply<int> _r = _t->SetValue((*reinterpret_cast< const QDBusVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QDBusPendingReply<int>*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VBusItemProxy::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VBusItemProxy::staticMetaObject = {
    { &QDBusAbstractInterface::staticMetaObject, qt_meta_stringdata_VBusItemProxy,
      qt_meta_data_VBusItemProxy, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VBusItemProxy::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VBusItemProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VBusItemProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VBusItemProxy))
        return static_cast<void*>(const_cast< VBusItemProxy*>(this));
    return QDBusAbstractInterface::qt_metacast(_clname);
}

int VBusItemProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        case 1: *reinterpret_cast< QDBusVariant*>(_v) = value(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: setValue(*reinterpret_cast< QDBusVariant*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void VBusItemProxy::PropertiesChanged(const QVariantMap & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
