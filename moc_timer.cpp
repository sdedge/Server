/****************************************************************************
** Meta object code from reading C++ file 'timer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "timer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'timer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Timer_t {
    QByteArrayData data[15];
    char stringdata0[204];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Timer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Timer_t qt_meta_stringdata_Timer = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Timer"
QT_MOC_LITERAL(1, 6, 8), // "finished"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 21), // "timeToUpdateVariables"
QT_MOC_LITERAL(4, 38, 25), // "timeToUpdateDropVariables"
QT_MOC_LITERAL(5, 64, 20), // "timeToCleanVariables"
QT_MOC_LITERAL(6, 85, 24), // "timeToCleanDropVariables"
QT_MOC_LITERAL(7, 110, 20), // "begin_drop_execution"
QT_MOC_LITERAL(8, 131, 3), // "run"
QT_MOC_LITERAL(9, 135, 20), // "insertNewWorkProgram"
QT_MOC_LITERAL(10, 156, 12), // "WorkProgram*"
QT_MOC_LITERAL(11, 169, 11), // "new_program"
QT_MOC_LITERAL(12, 181, 11), // "workProgram"
QT_MOC_LITERAL(13, 193, 4), // "drop"
QT_MOC_LITERAL(14, 198, 5) // "Drop*"

    },
    "Timer\0finished\0\0timeToUpdateVariables\0"
    "timeToUpdateDropVariables\0"
    "timeToCleanVariables\0timeToCleanDropVariables\0"
    "begin_drop_execution\0run\0insertNewWorkProgram\0"
    "WorkProgram*\0new_program\0workProgram\0"
    "drop\0Drop*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Timer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,
       4,    0,   66,    2, 0x06 /* Public */,
       5,    0,   67,    2, 0x06 /* Public */,
       6,    0,   68,    2, 0x06 /* Public */,
       7,    0,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   70,    2, 0x0a /* Public */,
       9,    1,   71,    2, 0x0a /* Public */,
      12,    0,   74,    2, 0x0a /* Public */,
      13,    0,   75,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    0x80000000 | 10,
    0x80000000 | 14,

       0        // eod
};

void Timer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Timer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->timeToUpdateVariables(); break;
        case 2: _t->timeToUpdateDropVariables(); break;
        case 3: _t->timeToCleanVariables(); break;
        case 4: _t->timeToCleanDropVariables(); break;
        case 5: _t->begin_drop_execution(); break;
        case 6: _t->run(); break;
        case 7: _t->insertNewWorkProgram((*reinterpret_cast< WorkProgram*(*)>(_a[1]))); break;
        case 8: { WorkProgram* _r = _t->workProgram();
            if (_a[0]) *reinterpret_cast< WorkProgram**>(_a[0]) = std::move(_r); }  break;
        case 9: { Drop* _r = _t->drop();
            if (_a[0]) *reinterpret_cast< Drop**>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Timer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Timer::finished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Timer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Timer::timeToUpdateVariables)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Timer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Timer::timeToUpdateDropVariables)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Timer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Timer::timeToCleanVariables)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Timer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Timer::timeToCleanDropVariables)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Timer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Timer::begin_drop_execution)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Timer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Timer.data,
    qt_meta_data_Timer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Timer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Timer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Timer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Timer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void Timer::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Timer::timeToUpdateVariables()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Timer::timeToUpdateDropVariables()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Timer::timeToCleanVariables()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Timer::timeToCleanDropVariables()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Timer::begin_drop_execution()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
