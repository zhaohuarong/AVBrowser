QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AVBrowser
TEMPLATE = app

DESTDIR = $$PWD/bin

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    setting.cpp \
    item.cpp \
    itemimagelabel.cpp \
    dialogresizepicture.cpp \
    dialogcroptime.cpp

HEADERS  += mainwindow.h \
    setting.h \
    item.h \
    define.h \
    itemimagelabel.h \
    dialogresizepicture.h \
    dialogcroptime.h

FORMS    += mainwindow.ui \
    item.ui \
    dialogresizepicture.ui \
    dialogcroptime.ui

RESOURCES += \
    res.qrc

TRANSLATIONS += $$PWD/zh_CN.ts

RC_FILE = app.rc
