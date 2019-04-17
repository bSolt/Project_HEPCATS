#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMovie>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>

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
}
// End Main Window
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::video_aurora()
{
    player = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    player->setMedia(QUrl::fromLocalFile("/home/colin/Project_HEPCATS/symposium_demo/videos/FromSCView.WMV"));

    player->setVideoOutput(ui->STK_Movie);
    player->play();

    // Set check statement for the aurora
    ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">Detecting . . .</span></p></body></html>");
    QPalette p = ui->is_aurora->palette(); // define pallete for textEdit..
    p.setColor(QPalette::Base, Qt::gray); // set color "Red" for textedit base
    p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
    ui->is_aurora->setPalette(p); // change textedit palette

    qint64 aurora_present = 100;

    if (aurora_present >= 50) // Aurora Present
    {
        ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">Aurora Present</span></p></body></html>");
        p = ui->is_aurora->palette(); // define pallete for textEdit..
        p.setColor(QPalette::Base, Qt::green); // set color "Red" for textedit base
        p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
        ui->is_aurora->setPalette(p); // change textedit palette
    }

    if(aurora_present < 50) //Aurora Not Present
    {
        ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">No Aurora Present . . .</span></p></body></html>");
        p = ui->is_aurora->palette(); // define pallete for textEdit..
        p.setColor(QPalette::Base, Qt::red); // set color "Red" for textedit base
        p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
        ui->is_aurora->setPalette(p); // change textedit palette
    }

}

void MainWindow::video_no_aurora()
{
    player = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    player->setMedia(QUrl::fromLocalFile("/home/colin/Project_HEPCATS/symposium_demo/videos/Video1.WMV"));

    player->setVideoOutput(ui->STK_Movie);
    player->play();

    // Set check statement for the aurora
    ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">Detecting . . .</span></p></body></html>");
    QPalette p = ui->is_aurora->palette(); // define pallete for textEdit..
    p.setColor(QPalette::Base, Qt::gray); // set color "Red" for textedit base
    p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
    ui->is_aurora->setPalette(p); // change textedit palette

    qint64 aurora_present = 1;

    if (aurora_present >= 50) // Aurora Present
    {
        ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">Aurora Present</span></p></body></html>");
        p = ui->is_aurora->palette(); // define pallete for textEdit..
        p.setColor(QPalette::Base, Qt::green); // set color "Red" for textedit base
        p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
        ui->is_aurora->setPalette(p); // change textedit palette
    }

    if(aurora_present < 50) //Aurora Not Present
    {
        ui->is_aurora->setHtml("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> <html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\"> p, li { white-space: pre-wrap; } </style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\"> <p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p> <p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">No Aurora Present . . .</span></p></body></html>");
        p = ui->is_aurora->palette(); // define pallete for textEdit..
        p.setColor(QPalette::Base, Qt::red); // set color "Red" for textedit base
        p.setColor(QPalette::Text, Qt::black); // set text color which is selected from color pallete
        ui->is_aurora->setPalette(p); // change textedit palette
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
