QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AVBrowser
TEMPLATE = app

DESTDIR = $$PWD/bin

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    setting.cpp \
    item.cpp

HEADERS  += mainwindow.h \
    setting.h \
    item.h \
    define.h

FORMS    += mainwindow.ui \
    item.ui

RESOURCES += \
    res.qrc

RC_FILE = app.rc
