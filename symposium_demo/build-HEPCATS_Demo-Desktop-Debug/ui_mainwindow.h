/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <qvideowidget.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btn_dont_include_aurora;
    QPushButton *btn_include_aurora;
    QTextEdit *is_aurora;
    QLabel *label_2;
    QVideoWidget *STK_Movie;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *play_movie;
    QPushButton *reset_movie;
    QMenuBar *menuBar;
    QMenu *menuProject_HEPCATS_Demo;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(904, 700);
        MainWindow->setMinimumSize(QSize(700, 700));
        MainWindow->setDockNestingEnabled(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(170, 10, 661, 38));
        QFont font;
        font.setFamily(QStringLiteral("Verdana"));
        font.setPointSize(24);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        horizontalLayoutWidget = new QWidget(centralWidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 580, 461, 51));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btn_dont_include_aurora = new QPushButton(horizontalLayoutWidget);
        btn_dont_include_aurora->setObjectName(QStringLiteral("btn_dont_include_aurora"));
        QFont font1;
        font1.setFamily(QStringLiteral("Vemana2000"));
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        btn_dont_include_aurora->setFont(font1);

        horizontalLayout->addWidget(btn_dont_include_aurora);

        btn_include_aurora = new QPushButton(horizontalLayoutWidget);
        btn_include_aurora->setObjectName(QStringLiteral("btn_include_aurora"));
        btn_include_aurora->setFont(font1);

        horizontalLayout->addWidget(btn_include_aurora);

        is_aurora = new QTextEdit(centralWidget);
        is_aurora->setObjectName(QStringLiteral("is_aurora"));
        is_aurora->setGeometry(QRect(490, 530, 391, 101));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        QBrush brush1(QColor(243, 243, 243, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        is_aurora->setPalette(palette);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(190, 510, 161, 17));
        label_2->setFont(font1);
        STK_Movie = new QVideoWidget(centralWidget);
        STK_Movie->setObjectName(QStringLiteral("STK_Movie"));
        STK_Movie->setGeometry(QRect(9, 50, 871, 451));
        horizontalLayoutWidget_2 = new QWidget(centralWidget);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(10, 530, 461, 41));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        play_movie = new QPushButton(horizontalLayoutWidget_2);
        play_movie->setObjectName(QStringLiteral("play_movie"));

        horizontalLayout_2->addWidget(play_movie);

        reset_movie = new QPushButton(horizontalLayoutWidget_2);
        reset_movie->setObjectName(QStringLiteral("reset_movie"));

        horizontalLayout_2->addWidget(reset_movie);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 904, 27));
        menuProject_HEPCATS_Demo = new QMenu(menuBar);
        menuProject_HEPCATS_Demo->setObjectName(QStringLiteral("menuProject_HEPCATS_Demo"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuProject_HEPCATS_Demo->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        label->setText(QApplication::translate("MainWindow", "Project HEPCATS Symposium Demo", 0));
        btn_dont_include_aurora->setText(QApplication::translate("MainWindow", "Don't Include Aurora", 0));
        btn_include_aurora->setText(QApplication::translate("MainWindow", "Include Aurora", 0));
        is_aurora->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:24pt; font-weight:600;\">Waiting on Video</span></p></body></html>", 0));
        label_2->setText(QApplication::translate("MainWindow", "Video Settings", 0));
        play_movie->setText(QApplication::translate("MainWindow", "Play/Pause", 0));
        reset_movie->setText(QApplication::translate("MainWindow", "Reset Video", 0));
        menuProject_HEPCATS_Demo->setTitle(QApplication::translate("MainWindow", "Project HEPCATS Demo", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
