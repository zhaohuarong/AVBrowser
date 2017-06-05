QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AVBrowser
TEMPLATE = app

DESTDIR = $$PWD/bin

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
    setting.cpp

HEADERS  += mainwindow.h \
    setting.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
