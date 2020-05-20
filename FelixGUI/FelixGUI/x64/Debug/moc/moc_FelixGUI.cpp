/****************************************************************************
** Meta object code from reading C++ file 'FelixGUI.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../FelixGUI.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FelixGUI.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MyActions_t {
    QByteArrayData data[4];
    char stringdata0[53];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MyActions_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MyActions_t qt_meta_stringdata_MyActions = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MyActions"
QT_MOC_LITERAL(1, 10, 26), // "on_action_select_directory"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 14) // "on_action_exit"

    },
    "MyActions\0on_action_select_directory\0"
    "\0on_action_exit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MyActions[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    0,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MyActions::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MyActions *_t = static_cast<MyActions *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_action_select_directory(); break;
        case 1: _t->on_action_exit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject MyActions::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MyActions.data,
      qt_meta_data_MyActions,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MyActions::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyActions::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyActions.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MyActions::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_FelixGUI_t {
    QByteArrayData data[16];
    char stringdata0[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FelixGUI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FelixGUI_t qt_meta_stringdata_FelixGUI = {
    {
QT_MOC_LITERAL(0, 0, 8), // "FelixGUI"
QT_MOC_LITERAL(1, 9, 10), // "addToTable"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 5), // "index"
QT_MOC_LITERAL(4, 27, 14), // "const wchar_t*"
QT_MOC_LITERAL(5, 42, 4), // "path"
QT_MOC_LITERAL(6, 47, 11), // "const char*"
QT_MOC_LITERAL(7, 59, 4), // "mode"
QT_MOC_LITERAL(8, 64, 8), // "wchar_t*"
QT_MOC_LITERAL(9, 73, 4), // "time"
QT_MOC_LITERAL(10, 78, 6), // "isFile"
QT_MOC_LITERAL(11, 85, 16), // "on_action_launch"
QT_MOC_LITERAL(12, 102, 14), // "addToProcTable"
QT_MOC_LITERAL(13, 117, 9), // "close_tab"
QT_MOC_LITERAL(14, 127, 16), // "on_action_inject"
QT_MOC_LITERAL(15, 144, 14) // "on_action_view"

    },
    "FelixGUI\0addToTable\0\0index\0const wchar_t*\0"
    "path\0const char*\0mode\0wchar_t*\0time\0"
    "isFile\0on_action_launch\0addToProcTable\0"
    "close_tab\0on_action_inject\0on_action_view"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FelixGUI[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    5,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   55,    2, 0x0a /* Public */,
      12,    5,   56,    2, 0x0a /* Public */,
      13,    1,   67,    2, 0x0a /* Public */,
      14,    0,   70,    2, 0x0a /* Public */,
      15,    0,   71,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 4, 0x80000000 | 6, 0x80000000 | 8, QMetaType::UInt,    3,    5,    7,    9,   10,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, 0x80000000 | 4, 0x80000000 | 6, 0x80000000 | 8, QMetaType::UInt,    3,    5,    7,    9,   10,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FelixGUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FelixGUI *_t = static_cast<FelixGUI *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addToTable((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const wchar_t*(*)>(_a[2])),(*reinterpret_cast< const char*(*)>(_a[3])),(*reinterpret_cast< wchar_t*(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5]))); break;
        case 1: _t->on_action_launch(); break;
        case 2: _t->addToProcTable((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< const wchar_t*(*)>(_a[2])),(*reinterpret_cast< const char*(*)>(_a[3])),(*reinterpret_cast< wchar_t*(*)>(_a[4])),(*reinterpret_cast< uint(*)>(_a[5]))); break;
        case 3: _t->close_tab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_action_inject(); break;
        case 5: _t->on_action_view(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (FelixGUI::*_t)(unsigned int , const wchar_t * , const char * , wchar_t * , unsigned int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&FelixGUI::addToTable)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject FelixGUI::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_FelixGUI.data,
      qt_meta_data_FelixGUI,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *FelixGUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FelixGUI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_FelixGUI.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int FelixGUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void FelixGUI::addToTable(unsigned int _t1, const wchar_t * _t2, const char * _t3, wchar_t * _t4, unsigned int _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
