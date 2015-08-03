/****************************************************************************
** Meta object code from reading C++ file 'QsLogDestFunctor.h'
**
** Created: Tue Jul 28 05:00:41 2015
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ext/qslog/QsLogDestFunctor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QsLogDestFunctor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QsLogging__FunctorDestination[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      45,   31,   30,   30, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QsLogging__FunctorDestination[] = {
    "QsLogging::FunctorDestination\0\0"
    "message,level\0logMessageReady(QString,int)\0"
};

void QsLogging::FunctorDestination::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FunctorDestination *_t = static_cast<FunctorDestination *>(_o);
        switch (_id) {
        case 0: _t->logMessageReady((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QsLogging::FunctorDestination::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QsLogging::FunctorDestination::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QsLogging__FunctorDestination,
      qt_meta_data_QsLogging__FunctorDestination, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QsLogging::FunctorDestination::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QsLogging::FunctorDestination::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QsLogging::FunctorDestination::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QsLogging__FunctorDestination))
        return static_cast<void*>(const_cast< FunctorDestination*>(this));
    if (!strcmp(_clname, "Destination"))
        return static_cast< Destination*>(const_cast< FunctorDestination*>(this));
    return QObject::qt_metacast(_clname);
}

int QsLogging::FunctorDestination::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QsLogging::FunctorDestination::logMessageReady(const QString & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
