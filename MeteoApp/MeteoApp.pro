QT       += core gui widgets sql charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    releve_dialog.h \
    attestation_dialog.h

FORMS += \
    mainwindow.ui \
    releve_dialog.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    schema.sql \
    style.qss

# Deployment par defaut
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
