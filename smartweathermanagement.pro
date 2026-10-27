QT += core gui widgets sql
CONFIG += c++17


SOURCES += \
    main.cpp \
    smartweather.cpp

HEADERS += \
    smartweather.h

FORMS += \
    smartweather.ui


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \0
    gitignore
