/****************************************************************************
** Meta object code from reading C++ file 'v_bus_node.h'
**
** Created: Tue Jul 28 05:00:57 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "src/v_bus_node.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'v_bus_node.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VBusNode[] = {

 // content:
       6,       // revision
       0,       // classname
       1,   14, // classinfo
       4,   16, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // classinfo: key, value
      35,    9,

 // signals: signature, parameters, type, tag, flags
      60,   52,   51,   51, 0x05,

 // slots: signature, parameters, type, tag, flags
     104,   51,   91,   51, 0x0a,
     115,   51,   51,   51, 0x08,
     131,   51,   51,   51, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VBusNode[] = {
    "VBusNode\0com.victronenergy.BusNode\0"
    "D-Bus Interface\0\0changes\0"
    "PropertiesChanged(QVariantMap)\0"
    "QDBusVariant\0GetValue()\0onItemDeleted()\0"
    "onNodeDeleted()\0"
};

void VBusNode::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VBusNode *_t = static_cast<VBusNode *>(_o);
        switch (_id) {
        case 0: _t->PropertiesChanged((*reinterpret_cast< const QVariantMap(*)>(_a[1]))); break;
        case 1: { QDBusVariant _r = _t->GetValue();
            if (_a[0]) *reinterpret_cast< QDBusVariant*>(_a[0]) = _r; }  break;
        case 2: _t->onItemDeleted(); break;
        case 3: _t->onNodeDeleted(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VBusNode::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VBusNode::staticMetaObject = {
    { &QDBusAbstractAdaptor::staticMetaObject, qt_meta_stringdata_VBusNode,
      qt_meta_data_VBusNode, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VBusNode::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VBusNode::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VBusNode::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VBusNode))
        return static_cast<void*>(const_cast< VBusNode*>(this));
    return QDBusAbstractAdaptor::qt_metacast(_clname);
}

int VBusNode::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDBusAbstractAdaptor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void VBusNode::PropertiesChanged(const QVariantMap & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
