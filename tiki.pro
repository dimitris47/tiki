QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    organizer.cpp \
    project.cpp \
    task.cpp

HEADERS += \
    dialog.h \
    mainwindow.h \
    organizer.h \
    project.h \
    task.h

FORMS += \
    dialog.ui \
    mainwindow.ui
