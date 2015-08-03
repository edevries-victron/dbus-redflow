/****************************************************************************
** Meta object code from reading C++ file 'v_busitem_adaptor.h'
**
** Created: Tue Jul 28 05:00:42 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ext/velib/src/qt/v_busitem_adaptor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'v_busitem_adaptor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VBusItemAdaptor[] = {

 // content:
       6,       // revision
       0,       // classname
       2,   14, // classinfo
       9,   18, // methods
       2,   63, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // classinfo: key, value
      42,   16,
    1334,   58,

 // signals: signature, parameters, type, tag, flags
    1363, 1355, 1354, 1354, 0x05,

 // slots: signature, parameters, type, tag, flags
    1407, 1354, 1394, 1354, 0x0a,
    1444, 1428, 1420, 1354, 0x0a,
    1472, 1354, 1394, 1354, 0x0a,
    1481, 1354, 1394, 1354, 0x0a,
    1490, 1354, 1420, 1354, 0x0a,
    1500, 1354, 1394, 1354, 0x0a,
    1515, 1354, 1511, 1354, 0x0a,
    1534, 1528, 1511, 1354, 0x0a,

 // properties: name, type, flags
    1557, 1420, 0x0a095001,
    1562, 1394, 0x0009510b,

       0        // eod
};

static const char qt_meta_stringdata_VBusItemAdaptor[] = {
    "VBusItemAdaptor\0com.victronenergy.BusItem\0"
    "D-Bus Interface\0"
    "  <interface name=\"com.victronenergy.BusItem\">\n    <method name=\"G"
    "etDescription\">\n      <arg direction=\"in\" type=\"s\" name=\"langua"
    "ge\"/>\n      <arg direction=\"in\" type=\"i\" name=\"length\"/>\n    "
    "  <arg direction=\"out\" type=\"s\" name=\"descr\"/>\n    </method>\n "
    "   <method name=\"GetValue\">\n      <arg direction=\"out\" type=\"v\""
    " name=\"value\"/>\n    </method>\n    <method name=\"GetText\">\n     "
    " <arg direction=\"out\" type=\"s\" name=\"value\"/>\n    </method>\n  "
    "  <method name=\"SetValue\">\n      <arg direction=\"in\" type=\"v\" n"
    "ame=\"value\"/>\n      <arg direction=\"out\" type=\"i\" name=\"retval"
    "\"/>\n    </method>\n    <method name=\"GetMin\">\n      <arg directio"
    "n=\"out\" type=\"v\" name=\"value\"/>\n    </method>\n    <method name"
    "=\"GetMax\">\n      <arg direction=\"out\" type=\"v\" name=\"value\"/>"
    "\n    </method>\n    <method name=\"SetDefault\">\n      <arg directio"
    "n=\"out\" type=\"i\" name=\"retval\"/>\n    </method>\n    <method nam"
    "e=\"GetDefault\">\n      <arg direction=\"out\" type=\"v\" name=\"valu"
    "e\"/>\n    </method>\n    <property access=\"readwrite\" type=\"v\" na"
    "me=\"Value\"/>\n    <property access=\"read\" type=\"s\" name=\"Text\""
    "/>\n    <signal name=\"PropertiesChanged\">\n      <arg direction=\"ou"
    "t\" type=\"a{sv}\" name=\"changes\"/>\n      <annotation value=\"QVari"
    "antMap\" name=\"com.trolltech.QtDBus.QtTypeName.In0\"/>\n    </signal>"
    "\n  </interface>\n\0"
    "D-Bus Introspection\0\0changes\0"
    "PropertiesChanged(QVariantMap)\0"
    "QDBusVariant\0GetDefault()\0QString\0"
    "language,length\0GetDescription(QString,int)\0"
    "GetMax()\0GetMin()\0GetText()\0GetValue()\0"
    "int\0SetDefault()\0value\0SetValue(QDBusVariant)\0"
    "Text\0Value\0"
};

void VBusItemAdaptor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VBusItemAdaptor *_t = static_cast<VBusItemAdaptor *>(_o);
        switch (_id) {
        case 0: _t->PropertiesChanged((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 1: { QDBusVariant _r = _t->GetDefault();
            if (_a[0]) *reinterpret_cast< QDBusVariant*>(_a[0]) = _r; }  break;
        case 2: { QString _r = _t->GetDescription((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 3: { QDBusVariant _r = _t->GetMax();
            if (_a[0]) *reinterpret_cast< QDBusVariant*>(_a[0]) = _r; }  break;
        case 4: { QDBusVariant _r = _t->GetMin();
            if (_a[0]) *reinterpret_cast< QDBusVariant*>(_a[0]) = _r; }  break;
        case 5: { QString _r = _t->GetText();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 6: { QDBusVariant _r = _t->GetValue();
            if (_a[0]) *reinterpret_cast< QDBusVariant*>(_a[0]) = _r; }  break;
        case 7: { int _r = _t->SetDefault();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: { int _r = _t->SetValue((*reinterpret_cast< const QDBusVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VBusItemAdaptor::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VBusItemAdaptor::staticMetaObject = {
    { &QDBusAbstractAdaptor::staticMetaObject, qt_meta_stringdata_VBusItemAdaptor,
      qt_meta_data_VBusItemAdaptor, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VBusItemAdaptor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VBusItemAdaptor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VBusItemAdaptor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VBusItemAdaptor))
        return static_cast<void*>(const_cast< VBusItemAdaptor*>(this));
    return QDBusAbstractAdaptor::qt_metacast(_clname);
}

int VBusItemAdaptor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractAdaptor::qt_metacall(_c, _id, _a);
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
void VBusItemAdaptor::PropertiesChanged(const QVariantMap & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
