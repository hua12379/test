/****************************************************************************
** Meta object code from reading C++ file 'MpvPlayer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../player/MpvPlayer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MpvPlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MpvPlayer_t {
    QByteArrayData data[5];
    char stringdata0[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MpvPlayer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MpvPlayer_t qt_meta_stringdata_MpvPlayer = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MpvPlayer"
QT_MOC_LITERAL(1, 10, 10), // "mpv_events"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 15), // "my_mpv_event_id"
QT_MOC_LITERAL(4, 38, 11) // "onMpvEvents"

    },
    "MpvPlayer\0mpv_events\0\0my_mpv_event_id\0"
    "onMpvEvents"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MpvPlayer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   29,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 3,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 3,    2,    2,

       0        // eod
};

void MpvPlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MpvPlayer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mpv_events((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< my_mpv_event_id(*)>(_a[2]))); break;
        case 1: _t->onMpvEvents((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< my_mpv_event_id(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MpvPlayer::*)(int , my_mpv_event_id );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MpvPlayer::mpv_events)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MpvPlayer::staticMetaObject = { {
    &MpvInterface::staticMetaObject,
    qt_meta_stringdata_MpvPlayer.data,
    qt_meta_data_MpvPlayer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MpvPlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MpvPlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MpvPlayer.stringdata0))
        return static_cast<void*>(this);
    return MpvInterface::qt_metacast(_clname);
}

int MpvPlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MpvInterface::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MpvPlayer::mpv_events(int _t1, my_mpv_event_id _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
