#include "mainwindow.h"
#include <QApplication>
#include <QMovie>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <qmath.h>
#include <QtMath>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
