QT += core gui widgets sql printsupport
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
    modulealerteswidget.cpp

HEADERS += \
    employe.h \
    smartweather.h \
    alerte.h \
    predictiongel.h \
    statsview.h \
    logindialog.h \
    connection.h \
    modulealerteswidget.h

FORMS += \
    smartweather.ui \
    logindialog.ui \
    modulealerteswidget.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    gitignore \
    style.qss \
    schema_employe.sql

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
