/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[18];
    char stringdata[223];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 18), // "signal_showPlaying"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 12), // "uPlayingItem"
QT_MOC_LITERAL(4, 44, 7), // "bManual"
QT_MOC_LITERAL(5, 52, 17), // "signal_playFinish"
QT_MOC_LITERAL(6, 70, 16), // "slot_showPlaying"
QT_MOC_LITERAL(7, 87, 15), // "slot_playFinish"
QT_MOC_LITERAL(8, 103, 18), // "slot_buttonClicked"
QT_MOC_LITERAL(9, 122, 8), // "CButton*"
QT_MOC_LITERAL(10, 131, 17), // "slot_labelClicked"
QT_MOC_LITERAL(11, 149, 7), // "CLabel*"
QT_MOC_LITERAL(12, 157, 23), // "slot_progressMousePress"
QT_MOC_LITERAL(13, 181, 3), // "pos"
QT_MOC_LITERAL(14, 185, 10), // "timerEvent"
QT_MOC_LITERAL(15, 196, 12), // "QTimerEvent*"
QT_MOC_LITERAL(16, 209, 5), // "event"
QT_MOC_LITERAL(17, 215, 7) // "QEvent*"

    },
    "MainWindow\0signal_showPlaying\0\0"
    "uPlayingItem\0bManual\0signal_playFinish\0"
    "slot_showPlaying\0slot_playFinish\0"
    "slot_buttonClicked\0CButton*\0"
    "slot_labelClicked\0CLabel*\0"
    "slot_progressMousePress\0pos\0timerEvent\0"
    "QTimerEvent*\0event\0QEvent*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x06 /* Public */,
       5,    0,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   65,    2, 0x08 /* Private */,
       7,    0,   70,    2, 0x08 /* Private */,
       8,    1,   71,    2, 0x08 /* Private */,
      10,    1,   74,    2, 0x08 /* Private */,
      12,    2,   77,    2, 0x08 /* Private */,
      14,    1,   82,    2, 0x08 /* Private */,
      16,    1,   85,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::Bool,    3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::UInt, QMetaType::Bool,    3,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,    2,
    QMetaType::Void, 0x80000000 | 11,    2,
    QMetaType::Void, 0x80000000 | 11, QMetaType::QPoint,    2,   13,
    QMetaType::Void, 0x80000000 | 15,    2,
    QMetaType::Bool, 0x80000000 | 17,    2,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->signal_showPlaying((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->signal_playFinish(); break;
        case 2: _t->slot_showPlaying((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->slot_playFinish(); break;
        case 4: _t->slot_buttonClicked((*reinterpret_cast< CButton*(*)>(_a[1]))); break;
        case 5: _t->slot_labelClicked((*reinterpret_cast< CLabel*(*)>(_a[1]))); break;
        case 6: _t->slot_progressMousePress((*reinterpret_cast< CLabel*(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 7: _t->timerEvent((*reinterpret_cast< QTimerEvent*(*)>(_a[1]))); break;
        case 8: { bool _r = _t->event((*reinterpret_cast< QEvent*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CButton* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CLabel* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CLabel* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)(unsigned int , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::signal_showPlaying)) {
                *result = 0;
            }
        }
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::signal_playFinish)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    if (!strcmp(_clname, "IModelObserver"))
        return static_cast< IModelObserver*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::signal_showPlaying(unsigned int _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::signal_playFinish()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
