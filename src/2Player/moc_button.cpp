/****************************************************************************
** Meta object code from reading C++ file 'button.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "button.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'button.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CButton_t {
    QByteArrayData data[7];
    char stringdata[86];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CButton_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CButton_t qt_meta_stringdata_CButton = {
    {
QT_MOC_LITERAL(0, 0, 7), // "CButton"
QT_MOC_LITERAL(1, 8, 14), // "signal_clicked"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "CButton*"
QT_MOC_LITERAL(4, 33, 20), // "signal_doubleClicked"
QT_MOC_LITERAL(5, 54, 18), // "signal_contextMenu"
QT_MOC_LITERAL(6, 73, 12) // "slot_clicked"

    },
    "CButton\0signal_clicked\0\0CButton*\0"
    "signal_doubleClicked\0signal_contextMenu\0"
    "slot_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CButton[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,
       5,    1,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   43,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CButton *_t = static_cast<CButton *>(_o);
        switch (_id) {
        case 0: _t->signal_clicked((*reinterpret_cast< CButton*(*)>(_a[1]))); break;
        case 1: _t->signal_doubleClicked((*reinterpret_cast< CButton*(*)>(_a[1]))); break;
        case 2: _t->signal_contextMenu((*reinterpret_cast< CButton*(*)>(_a[1]))); break;
        case 3: _t->slot_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CButton* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CButton* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CButton* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CButton::*_t)(CButton * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CButton::signal_clicked)) {
                *result = 0;
            }
        }
        {
            typedef void (CButton::*_t)(CButton * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CButton::signal_doubleClicked)) {
                *result = 1;
            }
        }
        {
            typedef void (CButton::*_t)(CButton * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CButton::signal_contextMenu)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject CButton::staticMetaObject = {
    { &CWidget<QPushButton>::staticMetaObject, qt_meta_stringdata_CButton.data,
      qt_meta_data_CButton,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CButton::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CButton.stringdata))
        return static_cast<void*>(const_cast< CButton*>(this));
    return CWidget<QPushButton>::qt_metacast(_clname);
}

int CButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CWidget<QPushButton>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CButton::signal_clicked(CButton * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CButton::signal_doubleClicked(CButton * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CButton::signal_contextMenu(CButton * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
