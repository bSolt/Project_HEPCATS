#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMovie>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <qmath.h>
#include <QtMath>
#include <QDebug>
#include <QTimer>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Buttons
    connect(ui->btn_include_aurora, SIGNAL(clicked(bool)),this,SLOT(video_aurora()));
    connect(ui->btn_dont_include_aurora, SIGNAL(clicked(bool)),this,SLOT(video_no_aurora()));
    connect(ui->play_movie,SIGNAL(clicked(bool)),this,SLOT(playpause_movie()));
    connect(ui->reset_movie,SIGNAL(clicked(bool)),this,SLOT(reset_movie()));

    double test = 0.00;
    QString stringtest = QString::number(test);
    ui->is_aurora->append("<span style=\" font-size:12pt; font-weight:600; color:#000000;\"> Probability: "+stringtest+"%</span>");


}
// End Main Window
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::video_aurora()
{ 
    int state;
    qint64 variable;
    if (player!=0x0)
        state=player->state();
    else{
        variable=1;
        state=2;
    }
    switch (state) {
    case 1: // Is playing
        variable=player->position();
        break;
    case 0: // Is not playing, do nothing, start at beginning of video
       variable=1;
        break;
     default:
        break;
    }
    player = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    player->setMedia(QUrl::fromLocalFile("/home/colin/Project_HEPCATS/symposium_demo/videos/MovieWithAurora.wmv"));
    player->setPosition(variable);
    player->setVideoOutput(ui->STK_Movie);
    player->play();

    timer=new QProcess(this);
    if (timer->state() == QProcess::Running)
        timer->terminate();
    //timer=new QProcess(this);
    connect(timer,SIGNAL(readyReadStandardOutput()),this,SLOT(aurora_prob_listeners()));
    timer->start("/home/colin/Project_HEPCATS/symposium_demo/bin/timer.sh");
    timer->waitForFinished(1);
}

void MainWindow::aurora_prob_listeners()
{
    // Set check statement for the aurora
    ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">Detecting . . .</span></p></body></html>");
    QPalette p = ui->is_aurora->palette(); // define pallete for textEdit..
    p.setColor(QPalette::Base, Qt::gray); // set color "Red" for textedit base
    p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
    ui->is_aurora->setPalette(p); // change textedit palette

    QVector<double> aurora_prob;
    aurora_prob.append(98.50); // 1
    aurora_prob.append(99.86); // 2
    aurora_prob.append(94.80); // 3
    aurora_prob.append(0.60); // 4
    aurora_prob.append(99.77); // 5
    aurora_prob.append(99.97); // 6
    aurora_prob.append(96.42); // 7
    aurora_prob.append(95.52); // 8
    aurora_prob.append(97.6); // 9
    aurora_prob.append(99.23); // 10
    aurora_prob.append(99.54); // 11
    aurora_prob.append(93.08); // 12

    qreal timestamp = player->position();
    qreal index = qFloor(timestamp/5001);

    double aurora_present = aurora_prob[index];

    if (aurora_present >= 50) // Aurora Present
    {
        ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:20pt; font-weight:600;\">Aurora Present</span></p></body></html>");
        p = ui->is_aurora->palette(); // define pallete for textEdit..
        p.setColor(QPalette::Base, Qt::green); // set color "Red" for textedit base
        p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
        ui->is_aurora->setPalette(p); // change textedit palette

        QString stringtemp = QString::number(aurora_present,'f',2);
        ui->is_aurora->append("<span style=\" font-size:12pt; font-weight:600; color:#000000;\"> Probability: "+stringtemp+"%</span>");
    }


    if(aurora_present < 50) //Aurora Not Present
    {
        ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">No Aurora Present</span></p></body></html>");
        p = ui->is_aurora->palette(); // define pallete for textEdit..
        p.setColor(QPalette::Base, Qt::red); // set color "Red" for textedit base
        p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
        ui->is_aurora->setPalette(p); // change textedit palette

        QString stringtemp = QString::number(aurora_present,'f',2);
        ui->is_aurora->append("<span style=\" font-size:12pt; font-weight:600; color:#000000;\"> Probability: "+stringtemp+"%</span>");
    }

}

void MainWindow::video_no_aurora()
{
    if (timer->state() == 2)
        timer->terminate();
    // Check to see if a video is playing and at what time it is
    int state;
    qint64 variable;
    if (player!=0x0)
        state=player->state();
    else{
        variable=1;
        state=2;
    }
    switch (state) {
    case 1: // Is playing
        variable=player->position();
        break;
    case 0: // Is not playing, do nothing, start at beginning of video
       variable=1;
        break;
     default:
        break;
    }

    player = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    player->setMedia(QUrl::fromLocalFile("/home/colin/Project_HEPCATS/symposium_demo/videos/MovieNoAurora_8_min.wmv"));

    player->setVideoOutput(ui->STK_Movie);
    player->play();
    player->setPosition(variable);

    timer=new QProcess(this);
    //timer=new QProcess(this);
    connect(timer,SIGNAL(readyReadStandardOutput()),this,SLOT(no_aurora_probs_listeners()));
    timer->start("/home/colin/Project_HEPCATS/symposium_demo/bin/timer.sh");
    timer->waitForFinished(1);
}

void MainWindow::no_aurora_probs_listeners()
{
// Set check statement for the aurora
ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">Detecting . . .</span></p></body></html>");
QPalette p = ui->is_aurora->palette(); // define pallete for textEdit..
p.setColor(QPalette::Base, Qt::gray); // set color "Red" for textedit base
p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
ui->is_aurora->setPalette(p); // change textedit palette

QVector<double> aurora_prob;
aurora_prob.append(0.91); // 1
aurora_prob.append(0.04); // 2
aurora_prob.append(0.01); // 3
aurora_prob.append(0.18); // 4
aurora_prob.append(0.03); // 5
aurora_prob.append(11.11); // 6
aurora_prob.append(6.71); // 7
aurora_prob.append(95.3); // 8
aurora_prob.append(83.66); // 9
aurora_prob.append(22.49); // 10
aurora_prob.append(99.78); // 11
aurora_prob.append(99.81); // 12
aurora_prob.append(92.38); // 13
aurora_prob.append(64.50); // 14
aurora_prob.append(37.91); // 15
aurora_prob.append(1.38); // 16


// Check where in the video we are
qreal timestamp = player->position();
qreal index = qFloor(timestamp/300000);

double aurora_present = aurora_prob[index];

if (aurora_present >= 50) // Aurora Present
{
    ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:20pt; font-weight:600;\">Aurora Present</span></p></body></html>");
    p = ui->is_aurora->palette(); // define pallete for textEdit..
    p.setColor(QPalette::Base, Qt::green); // set color "Red" for textedit base
    p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
    ui->is_aurora->setPalette(p); // change textedit palette

    QString stringtemp = QString::number(aurora_present,'f',2);
    ui->is_aurora->append("<span style=\" font-size:12pt; font-weight:600; color:#000000;\"> Probability: "+stringtemp+"%</span>");
}


if(aurora_present < 50) //Aurora Not Present
{
    ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">No Aurora Present</span></p></body></html>");
    p = ui->is_aurora->palette(); // define pallete for textEdit..
    p.setColor(QPalette::Base, Qt::red); // set color "Red" for textedit base
    p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
    ui->is_aurora->setPalette(p); // change textedit palette

    QString stringtemp = QString::number(aurora_present,'f',2);
    ui->is_aurora->append("<span style=\" font-size:12pt; font-weight:600; color:#000000;\"> Probability: "+stringtemp+"%</span>");
}
}

// Play or Pause the Movie
void MainWindow::playpause_movie()
{
    switch (player->state()) {
    case QMediaPlayer::PlayingState:
        player->pause();
        break;
    default:
        player->play();
        break;
    }
}

void MainWindow::reset_movie()
{
    player->setPosition(0);
}
