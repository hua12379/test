/****************************************************************************
** Meta object code from reading C++ file 'playmain.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../playmain.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playmain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_playMain_t {
    QByteArrayData data[11];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_playMain_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_playMain_t qt_meta_stringdata_playMain = {
    {
QT_MOC_LITERAL(0, 0, 8), // "playMain"
QT_MOC_LITERAL(1, 9, 11), // "onPlayVideo"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 11), // "onShotVideo"
QT_MOC_LITERAL(4, 34, 11), // "onStopVideo"
QT_MOC_LITERAL(5, 46, 10), // "onSetVideo"
QT_MOC_LITERAL(6, 57, 11), // "onQuitVideo"
QT_MOC_LITERAL(7, 69, 16), // "handle_mpv_event"
QT_MOC_LITERAL(8, 86, 6), // "handle"
QT_MOC_LITERAL(9, 93, 15), // "my_mpv_event_id"
QT_MOC_LITERAL(10, 109, 7) // "eventId"

    },
    "playMain\0onPlayVideo\0\0onShotVideo\0"
    "onStopVideo\0onSetVideo\0onQuitVideo\0"
    "handle_mpv_event\0handle\0my_mpv_event_id\0"
    "eventId"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_playMain[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x08 /* Private */,
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    0,   48,    2, 0x08 /* Private */,
       7,    2,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 9,    8,   10,

       0        // eod
};

void playMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<playMain *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onPlayVideo(); break;
        case 1: _t->onShotVideo(); break;
        case 2: _t->onStopVideo(); break;
        case 3: _t->onSetVideo(); break;
        case 4: _t->onQuitVideo(); break;
        case 5: _t->handle_mpv_event((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< my_mpv_event_id(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject playMain::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_playMain.data,
    qt_meta_data_playMain,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *playMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *playMain::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_playMain.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int playMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
