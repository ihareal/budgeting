#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "include/rapidjson/document.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Windows"));
    qDebug() << QStyleFactory::keys();
    MainWindow w;   
    w.show();
    return a.exec();
}
