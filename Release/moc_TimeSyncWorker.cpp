/****************************************************************************
** Meta object code from reading C++ file 'TimeSyncWorker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../TimeSyncWorker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TimeSyncWorker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TimeSyncWorker_t {
    QByteArrayData data[10];
    char stringdata0[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TimeSyncWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TimeSyncWorker_t qt_meta_stringdata_TimeSyncWorker = {
    {
QT_MOC_LITERAL(0, 0, 14), // "TimeSyncWorker"
QT_MOC_LITERAL(1, 15, 10), // "timeParsed"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 4), // "time"
QT_MOC_LITERAL(4, 32, 10), // "parseError"
QT_MOC_LITERAL(5, 43, 5), // "error"
QT_MOC_LITERAL(6, 49, 14), // "processUdpData"
QT_MOC_LITERAL(7, 64, 4), // "data"
QT_MOC_LITERAL(8, 69, 14), // "processDdsData"
QT_MOC_LITERAL(9, 84, 20) // "std::vector<uint8_t>"

    },
    "TimeSyncWorker\0timeParsed\0\0time\0"
    "parseError\0error\0processUdpData\0data\0"
    "processDdsData\0std::vector<uint8_t>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TimeSyncWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   40,    2, 0x0a /* Public */,
       8,    1,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QDateTime,    3,
    QMetaType::Void, QMetaType::QString,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Void, 0x80000000 | 9,    7,

       0        // eod
};

void TimeSyncWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TimeSyncWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->timeParsed((*reinterpret_cast< const QDateTime(*)>(_a[1]))); break;
        case 1: _t->parseError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->processUdpData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: _t->processDdsData((*reinterpret_cast< const std::vector<uint8_t>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TimeSyncWorker::*)(const QDateTime & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TimeSyncWorker::timeParsed)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TimeSyncWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TimeSyncWorker::parseError)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TimeSyncWorker::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_TimeSyncWorker.data,
    qt_meta_data_TimeSyncWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TimeSyncWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TimeSyncWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TimeSyncWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TimeSyncWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void TimeSyncWorker::timeParsed(const QDateTime & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TimeSyncWorker::parseError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
