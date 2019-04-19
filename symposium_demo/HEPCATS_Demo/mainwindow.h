#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <qmath.h>
#include <QtMath>
#include <QDebug>
#include <QTimer>
#include <QProcess>

class QAbstractItemView;
class QLabel;
class QMediaPlayer;
class QModelIndex;
class QPushButton;
class QSlider;
class QStatusBar;
class QVideoProbe;
class QVideoWidget;
class QAudioProbe;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void video_aurora();
    void video_no_aurora();
    void playpause_movie();
    void reset_movie();
    void aurora_prob_listeners();
    void no_aurora_probs_listeners();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QProcess *timer;
    QProcess *timer_no;
};
#endif // MAINWINDOW_H
