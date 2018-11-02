/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_9;
    QLabel *ContactName;
    QTabWidget *tabWidget;
    QWidget *tab_2;
    QLabel *label_6;
    QComboBox *comboBox;
    QPushButton *pushButton_2;
    QWidget *CommandingTab;
    QPushButton *SendCommand;
    QGroupBox *groupBox_2;
    QListWidget *CommandList;
    QLabel *label_4;
    QLabel *label_8;
    QLabel *CurrentCommand;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QLabel *SyntaxStatus;
    QLabel *label_5;
    QLabel *CommandToSend;
    QPushButton *pushButton_3;
    QGroupBox *groupBox_3;
    QListWidget *listWidget;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QWidget *tab;
    QLabel *label_11;
    QLabel *label_12;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_3;
    QComboBox *UplinkScale;
    QComboBox *DownlinkScale;
    QGroupBox *groupBox;
    QLabel *label_7;
    QLabel *label_10;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_21;
    QLabel *label_22;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1149, 835);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(982, 20, 71, 16));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(982, 50, 81, 16));
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(480, 30, 55, 16));
        ContactName = new QLabel(centralWidget);
        ContactName->setObjectName(QStringLiteral("ContactName"));
        ContactName->setGeometry(QRect(540, 30, 55, 16));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(0, 60, 951, 701));
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        label_6 = new QLabel(tab_2);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 30, 55, 16));
        comboBox = new QComboBox(tab_2);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(100, 30, 101, 22));
        pushButton_2 = new QPushButton(tab_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(80, 80, 121, 28));
        tabWidget->addTab(tab_2, QString());
        CommandingTab = new QWidget();
        CommandingTab->setObjectName(QStringLiteral("CommandingTab"));
        SendCommand = new QPushButton(CommandingTab);
        SendCommand->setObjectName(QStringLiteral("SendCommand"));
        SendCommand->setGeometry(QRect(830, 630, 93, 28));
        QPalette palette;
        QBrush brush(QColor(170, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush1(QColor(214, 206, 200, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        SendCommand->setPalette(palette);
        groupBox_2 = new QGroupBox(CommandingTab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(20, 20, 361, 531));
        CommandList = new QListWidget(groupBox_2);
        new QListWidgetItem(CommandList);
        new QListWidgetItem(CommandList);
        new QListWidgetItem(CommandList);
        new QListWidgetItem(CommandList);
        new QListWidgetItem(CommandList);
        CommandList->setObjectName(QStringLiteral("CommandList"));
        CommandList->setGeometry(QRect(30, 40, 256, 192));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 20, 111, 16));
        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(40, 240, 111, 16));
        CurrentCommand = new QLabel(groupBox_2);
        CurrentCommand->setObjectName(QStringLiteral("CurrentCommand"));
        CurrentCommand->setGeometry(QRect(130, 240, 121, 16));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(40, 270, 91, 16));
        lineEdit = new QLineEdit(groupBox_2);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(40, 290, 251, 22));
        pushButton = new QPushButton(groupBox_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(110, 320, 93, 28));
        SyntaxStatus = new QLabel(groupBox_2);
        SyntaxStatus->setObjectName(QStringLiteral("SyntaxStatus"));
        SyntaxStatus->setGeometry(QRect(220, 330, 91, 16));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 360, 91, 16));
        CommandToSend = new QLabel(groupBox_2);
        CommandToSend->setObjectName(QStringLiteral("CommandToSend"));
        CommandToSend->setGeometry(QRect(20, 380, 261, 16));
        pushButton_3 = new QPushButton(groupBox_2);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(110, 420, 93, 28));
        groupBox_3 = new QGroupBox(CommandingTab);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(430, 30, 371, 521));
        listWidget = new QListWidget(groupBox_3);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(20, 20, 331, 271));
        label_17 = new QLabel(groupBox_3);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(30, 310, 71, 16));
        label_18 = new QLabel(groupBox_3);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(110, 310, 81, 16));
        label_19 = new QLabel(groupBox_3);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(30, 340, 71, 16));
        label_20 = new QLabel(groupBox_3);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(30, 360, 55, 16));
        tabWidget->addTab(CommandingTab, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        label_11 = new QLabel(tab);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(40, 30, 91, 16));
        label_12 = new QLabel(tab);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(40, 60, 91, 16));
        lineEdit_2 = new QLineEdit(tab);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(140, 30, 113, 22));
        lineEdit_3 = new QLineEdit(tab);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(140, 60, 113, 22));
        UplinkScale = new QComboBox(tab);
        UplinkScale->addItem(QString());
        UplinkScale->addItem(QString());
        UplinkScale->addItem(QString());
        UplinkScale->setObjectName(QStringLiteral("UplinkScale"));
        UplinkScale->setGeometry(QRect(260, 30, 79, 22));
        DownlinkScale = new QComboBox(tab);
        DownlinkScale->addItem(QString());
        DownlinkScale->addItem(QString());
        DownlinkScale->addItem(QString());
        DownlinkScale->setObjectName(QStringLiteral("DownlinkScale"));
        DownlinkScale->setGeometry(QRect(260, 60, 79, 22));
        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 10, 331, 81));
        tabWidget->addTab(tab, QString());
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(620, 30, 55, 16));
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(660, 30, 55, 16));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_10->setFont(font);
        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(970, 100, 71, 16));
        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(970, 130, 81, 16));
        label_15 = new QLabel(centralWidget);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(1070, 100, 55, 16));
        label_16 = new QLabel(centralWidget);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(1070, 130, 55, 16));
        label_21 = new QLabel(centralWidget);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(1080, 20, 55, 16));
        label_22 = new QLabel(centralWidget);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(1080, 50, 55, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1149, 25));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QApplication::translate("MainWindow", "Uplink Count", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Recived Count", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "Contact:", nullptr));
        ContactName->setText(QApplication::translate("MainWindow", "HEPCAT 1", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "Contacts", nullptr));
        comboBox->setItemText(0, QApplication::translate("MainWindow", "HEPCATS1", nullptr));
        comboBox->setItemText(1, QApplication::translate("MainWindow", "HEPCATS2", nullptr));
        comboBox->setItemText(2, QApplication::translate("MainWindow", "HEPCATS3", nullptr));

        pushButton_2->setText(QApplication::translate("MainWindow", "Establish Contact", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Establish Contacts", nullptr));
        SendCommand->setText(QApplication::translate("MainWindow", "Send", nullptr));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Commands", nullptr));

        const bool __sortingEnabled = CommandList->isSortingEnabled();
        CommandList->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = CommandList->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("MainWindow", "Set Imaging Start Time", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = CommandList->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("MainWindow", "MemDump", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = CommandList->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("MainWindow", "Set Imaging End Time", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = CommandList->item(3);
        ___qlistwidgetitem3->setText(QApplication::translate("MainWindow", "Check Clock Sync", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = CommandList->item(4);
        ___qlistwidgetitem4->setText(QApplication::translate("MainWindow", "Set Imaging Rate", nullptr));
        CommandList->setSortingEnabled(__sortingEnabled);

        label_4->setText(QApplication::translate("MainWindow", "Command List", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "Commanding:", nullptr));
        CurrentCommand->setText(QApplication::translate("MainWindow", "SetImageROIStart", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Arguments:", nullptr));
        lineEdit->setText(QApplication::translate("MainWindow", "2458407.25448", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "Verify", nullptr));
        SyntaxStatus->setText(QApplication::translate("MainWindow", "Syntax OK", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Command Text", nullptr));
        CommandToSend->setText(QApplication::translate("MainWindow", "SetImageROIStart 2458407.25448 ", nullptr));
        pushButton_3->setText(QApplication::translate("MainWindow", "Lock", nullptr));
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Command History", nullptr));

        const bool __sortingEnabled1 = listWidget->isSortingEnabled();
        listWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem5 = listWidget->item(0);
        ___qlistwidgetitem5->setText(QApplication::translate("MainWindow", "SetImageROIStart", nullptr));
        QListWidgetItem *___qlistwidgetitem6 = listWidget->item(1);
        ___qlistwidgetitem6->setText(QApplication::translate("MainWindow", "SetImageROIStop", nullptr));
        QListWidgetItem *___qlistwidgetitem7 = listWidget->item(2);
        ___qlistwidgetitem7->setText(QApplication::translate("MainWindow", "SetImageRate", nullptr));
        QListWidgetItem *___qlistwidgetitem8 = listWidget->item(3);
        ___qlistwidgetitem8->setText(QApplication::translate("MainWindow", "CheckClock", nullptr));
        listWidget->setSortingEnabled(__sortingEnabled1);

        label_17->setText(QApplication::translate("MainWindow", "Time Sent", nullptr));
        label_18->setText(QApplication::translate("MainWindow", "03:58:14 UTC", nullptr));
        label_19->setText(QApplication::translate("MainWindow", "Arguments", nullptr));
        label_20->setText(QApplication::translate("MainWindow", "0.5", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(CommandingTab), QApplication::translate("MainWindow", "Commanding", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "Uplink Rate", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "Downlink Rate", nullptr));
        UplinkScale->setItemText(0, QApplication::translate("MainWindow", "Mbps", nullptr));
        UplinkScale->setItemText(1, QApplication::translate("MainWindow", "Kbps", nullptr));
        UplinkScale->setItemText(2, QApplication::translate("MainWindow", "Bitps", nullptr));

        DownlinkScale->setItemText(0, QApplication::translate("MainWindow", "Mbps", nullptr));
        DownlinkScale->setItemText(1, QApplication::translate("MainWindow", "Kbps", nullptr));
        DownlinkScale->setItemText(2, QApplication::translate("MainWindow", "Bitps", nullptr));

        groupBox->setTitle(QApplication::translate("MainWindow", "Command Rates", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Sim Constraints", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "Link:", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "Good", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "Uplink Rate", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "Downlink Rate", nullptr));
        label_15->setText(QApplication::translate("MainWindow", "56 kbps", nullptr));
        label_16->setText(QApplication::translate("MainWindow", "56 kbps", nullptr));
        label_21->setText(QApplication::translate("MainWindow", "156", nullptr));
        label_22->setText(QApplication::translate("MainWindow", "156", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
