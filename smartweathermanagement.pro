QT += core gui widgets sql printsupport charts
CONFIG += c++17

SOURCES += \
    employe.cpp \
    main.cpp \
    smartweather.cpp \
    alerte.cpp \
    predictiongel.cpp \
    statsview.cpp \
    logindialog.cpp \
    connection.cpp \
    modulealerteswidget.cpp \
    station.cpp \
    stationwindow.cpp \
    relevewindow.cpp \
    maintenancewindow.cpp \
    interventiondialog.cpp \
    localdb.cpp

HEADERS += \
    employe.h \
    smartweather.h \
    alerte.h \
    predictiongel.h \
    statsview.h \
    logindialog.h \
    connection.h \
    modulealerteswidget.h \
    station.h \
    stationwindow.h \
    relevewindow.h \
    releve_dialog.h \
    attestation_dialog.h \
    maintenancewindow.h \
    interventiondialog.h \
    style.h \
    dbcompat.h \
    localdb.h

FORMS += \
    smartweather.ui \
    logindialog.ui \
    modulealerteswidget.ui \
    stationwindow.ui \
    relevewindow.ui \
    releve_dialog.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    gitignore \
    style.qss

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
