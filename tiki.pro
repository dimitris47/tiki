QT += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = tiki
TEMPLATE = app
VERSION = 4.3
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    alerts.cpp \
    dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    organizer.cpp \
    project.cpp \
    task.cpp

HEADERS += \
    alerts.h \
    dialog.h \
    global.h \
    mainwindow.h \
    organizer.h \
    project.h \
    task.h

FORMS += \
    alerts.ui \
    dialog.ui \
    mainwindow.ui

unix:!android {
    target.path = $${PREFIX}/bin
    icon.path = /usr/share/pixmaps
    icon.files = icons/tiki.png
    desktop.path = /usr/share/applications
    desktop.files = tiki.desktop
    INSTALLS += target icon desktop
}

win32 {
    VERSION = $${VERSION}.0.0
    QMAKE_TARGET_COMPANY = DP Software
    QMAKE_TARGET_DESCRIPTION = Simple project organizer
    QMAKE_TARGET_COPYRIGHT = \\251 2021 Dimitris Psathas
    QMAKE_TARGET_PRODUCT = Tiki
    RC_ICONS = icons/tiki.ico
    RC_LANG = 0x408
    RC_CODEPAGE = 1253
}

DISTFILES += \
    icons/tiki.ico \
    icons/tiki.png \
    tiki.desktop

RESOURCES += \
    tiki.qrc
