QT       += core gui widgets sql charts printsupport
CONFIG   += c++17

TARGET    = MaintenanceInterventions
TEMPLATE  = app

SOURCES += \
    main.cpp \
    database.cpp \
    interventiondialog.cpp \
    mainwindow.cpp

HEADERS += \
    database.h \
    interventiondialog.h \
    mainwindow.h \
    style.h
