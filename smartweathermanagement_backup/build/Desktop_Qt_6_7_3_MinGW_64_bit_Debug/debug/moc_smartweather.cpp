/****************************************************************************
** Meta object code from reading C++ file 'smartweather.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../smartweather.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'smartweather.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSsmartweatherENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSsmartweatherENDCLASS = QtMocHelpers::stringData(
    "smartweather",
    "seConnecter",
    "",
    "deconnecter",
    "allerPage",
    "index",
    "rechercheGlobale",
    "rafraichirDashboard",
    "ajouter",
    "modifier",
    "supprimer",
    "charger",
    "exporterCsv",
    "exporterPdf",
    "afficherStats",
    "remplirFormulaire",
    "QModelIndex",
    "viderFormulaire"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSsmartweatherENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   98,    2, 0x08,    1 /* Private */,
       3,    0,   99,    2, 0x08,    2 /* Private */,
       4,    1,  100,    2, 0x08,    3 /* Private */,
       6,    0,  103,    2, 0x08,    5 /* Private */,
       7,    0,  104,    2, 0x08,    6 /* Private */,
       8,    0,  105,    2, 0x08,    7 /* Private */,
       9,    0,  106,    2, 0x08,    8 /* Private */,
      10,    0,  107,    2, 0x08,    9 /* Private */,
      11,    0,  108,    2, 0x08,   10 /* Private */,
      12,    0,  109,    2, 0x08,   11 /* Private */,
      13,    0,  110,    2, 0x08,   12 /* Private */,
      14,    0,  111,    2, 0x08,   13 /* Private */,
      15,    1,  112,    2, 0x08,   14 /* Private */,
      17,    0,  115,    2, 0x08,   16 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,    5,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject smartweather::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSsmartweatherENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSsmartweatherENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSsmartweatherENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<smartweather, std::true_type>,
        // method 'seConnecter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'deconnecter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'allerPage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'rechercheGlobale'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'rafraichirDashboard'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ajouter'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'modifier'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'supprimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'charger'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exporterCsv'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'exporterPdf'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'afficherStats'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remplirFormulaire'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'viderFormulaire'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void smartweather::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<smartweather *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->seConnecter(); break;
        case 1: _t->deconnecter(); break;
        case 2: _t->allerPage((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->rechercheGlobale(); break;
        case 4: _t->rafraichirDashboard(); break;
        case 5: _t->ajouter(); break;
        case 6: _t->modifier(); break;
        case 7: _t->supprimer(); break;
        case 8: _t->charger(); break;
        case 9: _t->exporterCsv(); break;
        case 10: _t->exporterPdf(); break;
        case 11: _t->afficherStats(); break;
        case 12: _t->remplirFormulaire((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 13: _t->viderFormulaire(); break;
        default: ;
        }
    }
}

const QMetaObject *smartweather::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *smartweather::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSsmartweatherENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int smartweather::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
