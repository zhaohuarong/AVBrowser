#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator t;
    t.load(":/zh_CN.qm");
    a.installTranslator(&t);

    MainWindow w;
    w.show();

    return a.exec();
}
