/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLineEdit *txt_prompt;
    QTextBrowser *cmd_txt_messages;
    QPushButton *btn_enter;
    QListWidget *lst_commands;
    QPushButton *btn_quit;
    QPushButton *btn_start_listen;
    QGroupBox *groupBox_2;
    QTextBrowser *txt_messages_out;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1101, 813);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(30, 30, 971, 351));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        txt_prompt = new QLineEdit(groupBox);
        txt_prompt->setObjectName(QStringLiteral("txt_prompt"));
        txt_prompt->setAcceptDrops(false);

        gridLayout->addWidget(txt_prompt, 3, 0, 1, 1);

        cmd_txt_messages = new QTextBrowser(groupBox);
        cmd_txt_messages->setObjectName(QStringLiteral("cmd_txt_messages"));
        cmd_txt_messages->setAcceptDrops(false);

        gridLayout->addWidget(cmd_txt_messages, 1, 0, 1, 1);

        btn_enter = new QPushButton(groupBox);
        btn_enter->setObjectName(QStringLiteral("btn_enter"));

        gridLayout->addWidget(btn_enter, 3, 1, 1, 1);

        lst_commands = new QListWidget(groupBox);
        new QListWidgetItem(lst_commands);
        lst_commands->setObjectName(QStringLiteral("lst_commands"));

        gridLayout->addWidget(lst_commands, 1, 1, 1, 1);

        btn_quit = new QPushButton(centralWidget);
        btn_quit->setObjectName(QStringLiteral("btn_quit"));
        btn_quit->setGeometry(QRect(1000, 720, 87, 29));
        btn_start_listen = new QPushButton(centralWidget);
        btn_start_listen->setObjectName(QStringLiteral("btn_start_listen"));
        btn_start_listen->setGeometry(QRect(10, 710, 171, 25));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(40, 380, 421, 281));
        txt_messages_out = new QTextBrowser(groupBox_2);
        txt_messages_out->setObjectName(QStringLiteral("txt_messages_out"));
        txt_messages_out->setGeometry(QRect(0, 30, 411, 241));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1101, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "HEPCATS Ground Control", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MainWindow", "Commanding", Q_NULLPTR));
        txt_prompt->setText(QString());
        txt_prompt->setPlaceholderText(QApplication::translate("MainWindow", "PROMPT>", Q_NULLPTR));
        btn_enter->setText(QApplication::translate("MainWindow", "Send", Q_NULLPTR));

        const bool __sortingEnabled = lst_commands->isSortingEnabled();
        lst_commands->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = lst_commands->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("MainWindow", "NOOP", Q_NULLPTR));
        lst_commands->setSortingEnabled(__sortingEnabled);

        btn_quit->setText(QApplication::translate("MainWindow", "Quit", Q_NULLPTR));
        btn_start_listen->setText(QApplication::translate("MainWindow", "Start Listeners", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Messages", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
