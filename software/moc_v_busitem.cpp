/****************************************************************************
** Meta object code from reading C++ file 'v_busitem.h'
**
** Created: Mon Jul 27 00:29:37 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ext/velib/inc/velib/qt/v_busitem.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'v_busitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VBusItem[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       7,   59, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      25,    9,    9,    9, 0x05,
      46,    9,    9,    9, 0x05,
      60,    9,    9,    9, 0x05,
      73,    9,    9,    9, 0x05,
      86,    9,    9,    9, 0x05,
     103,    9,    9,    9, 0x05,

 // methods: signature, parameters, type, tag, flags
     127,  121,  117,    9, 0x02,
     146,    9,    9,    9, 0x02,

 // properties: name, type, flags
     121,  159, 0xff495103,
     168,  159, 0xff495001,
     173,  159, 0xff495001,
     193,  185, 0x0a495103,
     198,  159, 0xff495001,
     202,  159, 0xff495001,
     206,  159, 0xff495001,

 // properties: notify_signal_id
       0,
       6,
       1,
       2,
       3,
       4,
       5,

       0        // eod
};

static const char qt_meta_stringdata_VBusItem[] = {
    "VBusItem\0\0valueChanged()\0descriptionChanged()\0"
    "bindChanged()\0minChanged()\0maxChanged()\0"
    "defaultChanged()\0textChanged()\0int\0"
    "value\0setValue(QVariant)\0setDefault()\0"
    "QVariant\0text\0description\0QString\0"
    "bind\0min\0max\0defaultValue\0"
};

void VBusItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VBusItem *_t = static_cast<VBusItem *>(_o);
        switch (_id) {
        case 0: _t->valueChanged(); break;
        case 1: _t->descriptionChanged(); break;
        case 2: _t->bindChanged(); break;
        case 3: _t->minChanged(); break;
        case 4: _t->maxChanged(); break;
        case 5: _t->defaultChanged(); break;
        case 6: _t->textChanged(); break;
        case 7: { int _r = _t->setValue((*reinterpret_cast< const QVariant(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: _t->setDefault(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VBusItem::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VBusItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VBusItem,
      qt_meta_data_VBusItem, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VBusItem::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VBusItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VBusItem::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VBusItem))
        return static_cast<void*>(const_cast< VBusItem*>(this));
    return QObject::qt_metacast(_clname);
}

int VBusItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
        case 0: *reinterpret_cast< QVariant*>(_v) = getValue(); break;
        case 1: *reinterpret_cast< QVariant*>(_v) = getText(); break;
        case 2: *reinterpret_cast< QVariant*>(_v) = getDescription(); break;
        case 3: *reinterpret_cast< QString*>(_v) = getBind(); break;
        case 4: *reinterpret_cast< QVariant*>(_v) = getMin(); break;
        case 5: *reinterpret_cast< QVariant*>(_v) = getMax(); break;
        case 6: *reinterpret_cast< QVariant*>(_v) = getDefault(); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setValue(*reinterpret_cast< QVariant*>(_v)); break;
        case 3: setBind(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void VBusItem::valueChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void VBusItem::descriptionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void VBusItem::bindChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void VBusItem::minChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void VBusItem::maxChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void VBusItem::defaultChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void VBusItem::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}
QT_END_MOC_NAMESPACE
