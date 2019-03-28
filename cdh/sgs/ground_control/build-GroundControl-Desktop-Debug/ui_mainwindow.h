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
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGroupBox *grp_box_cmd;
    QGridLayout *gridLayout;
    QLineEdit *txt_prompt;
    QPushButton *btn_enter;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *txt_cmd_add_cmd;
    QPushButton *btn_tab_cmd_add_cmd;
    QTextBrowser *cmd_txt_messages;
    QTreeWidget *tree_active_cmds;
    QGroupBox *grp_box_setup;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout_3;
    QLabel *label_23;
    QRadioButton *rad_btn_Master;
    QRadioButton *rad_btn_Monitor;
    QVBoxLayout *verticalLayout_2;
    QPushButton *btn_start_listen;
    QPushButton *btn_start_program;
    QPushButton *btn_start_ips;
    QPushButton *btn_stop_listeners;
    QGroupBox *grp_box_messages;
    QTextBrowser *txt_messages_out;
    QGroupBox *grp_box_telemetry;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout_2;
    QGroupBox *grp_box_cmd_count;
    QGridLayout *gridLayout_3;
    QFormLayout *formLayout;
    QLabel *label_3;
    QLabel *label;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_5;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *lbl_val_telecmd_pkt_cnt;
    QLabel *lbl_inv_telecmd_pkt_cnt;
    QLabel *lbl_val_cmd_cnt;
    QLabel *lbl_inv_cmd_cnt;
    QLabel *lbl_cmd_exec_suc_cnt;
    QLabel *lbl_cmd_exec_err_cnt;
    QLabel *lbl_tlm_pkt_xfr_frm_seq_cnt;
    QLabel *lbl_next_atc_tm_str;
    QLabel *lbl_pbk_prog_flg;
    QLabel *label_20;
    QLabel *lbl_rx_cmd_pkt_cnt;
    QLabel *lbl_link_state;
    QLabel *label_25;
    QGroupBox *grp_box_housekeeping;
    QWidget *formLayoutWidget_2;
    QFormLayout *formLayout_2;
    QLabel *label_21;
    QLabel *lbl_acq_img_cnt;
    QLabel *label_9;
    QLabel *lbl_img_acq_prog_flag;
    QLabel *label_10;
    QLabel *lbl_ers_rly_swtch_state;
    QLabel *label_11;
    QLabel *lbl_mdq_scan_state;
    QLabel *label_13;
    QLabel *lbl_flt_tbl_mode;
    QLabel *label_12;
    QLabel *lbl_img_accpt_cnt;
    QLabel *label_14;
    QLabel *lbl_img_rej_cnt;
    QLabel *label_2;
    QLabel *lbl_next_img_acq_tm_str;
    QLabel *label_17;
    QLabel *label_22;
    QLabel *lbl_sys_tm_str;
    QGroupBox *groupBox;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout_4;
    QLabel *label_24;
    QLabel *label_26;
    QLabel *label_27;
    QLabel *lbl_magx;
    QLabel *lbl_magy;
    QLabel *lbl_magz;
    QPushButton *btn_quit;
    QWidget *tab_2;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QTableWidget *tbl_cmd_list;
    QTreeWidget *tree_inactive_cmds;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *txt_browser_add_cmd;
    QPushButton *btn_add_cmd;
    QPushButton *btn_cmd_list_remove;
    QPushButton *btn_set_cmd_list_active;
    QWidget *horizontalLayoutWidget_4;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *btn_active_to_inactive;
    QPushButton *btn_add_all;
    QPushButton *btn_load_commands;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1516, 808);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 1491, 741));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        QPalette palette;
        QBrush brush(QColor(229, 229, 229, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        tab->setPalette(palette);
        grp_box_cmd = new QGroupBox(tab);
        grp_box_cmd->setObjectName(QStringLiteral("grp_box_cmd"));
        grp_box_cmd->setGeometry(QRect(40, 30, 971, 351));
        gridLayout = new QGridLayout(grp_box_cmd);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        txt_prompt = new QLineEdit(grp_box_cmd);
        txt_prompt->setObjectName(QStringLiteral("txt_prompt"));
        txt_prompt->setAcceptDrops(true);
        txt_prompt->setDragEnabled(false);
        txt_prompt->setClearButtonEnabled(true);

        gridLayout->addWidget(txt_prompt, 3, 0, 1, 1);

        btn_enter = new QPushButton(grp_box_cmd);
        btn_enter->setObjectName(QStringLiteral("btn_enter"));

        gridLayout->addWidget(btn_enter, 3, 1, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        txt_cmd_add_cmd = new QLineEdit(grp_box_cmd);
        txt_cmd_add_cmd->setObjectName(QStringLiteral("txt_cmd_add_cmd"));

        horizontalLayout_3->addWidget(txt_cmd_add_cmd);

        btn_tab_cmd_add_cmd = new QPushButton(grp_box_cmd);
        btn_tab_cmd_add_cmd->setObjectName(QStringLiteral("btn_tab_cmd_add_cmd"));
        btn_tab_cmd_add_cmd->setEnabled(true);

        horizontalLayout_3->addWidget(btn_tab_cmd_add_cmd);


        gridLayout->addLayout(horizontalLayout_3, 2, 1, 1, 1);

        cmd_txt_messages = new QTextBrowser(grp_box_cmd);
        cmd_txt_messages->setObjectName(QStringLiteral("cmd_txt_messages"));
        cmd_txt_messages->setAcceptDrops(false);

        gridLayout->addWidget(cmd_txt_messages, 1, 0, 2, 1);

        tree_active_cmds = new QTreeWidget(grp_box_cmd);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem(tree_active_cmds);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(tree_active_cmds);
        new QTreeWidgetItem(tree_active_cmds);
        new QTreeWidgetItem(tree_active_cmds);
        tree_active_cmds->setObjectName(QStringLiteral("tree_active_cmds"));
        tree_active_cmds->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tree_active_cmds->setDragEnabled(true);
        tree_active_cmds->setDragDropMode(QAbstractItemView::DragOnly);
        tree_active_cmds->setDefaultDropAction(Qt::CopyAction);

        gridLayout->addWidget(tree_active_cmds, 1, 1, 1, 1);

        grp_box_setup = new QGroupBox(tab);
        grp_box_setup->setObjectName(QStringLiteral("grp_box_setup"));
        grp_box_setup->setGeometry(QRect(1020, 50, 351, 341));
        verticalLayout = new QVBoxLayout(grp_box_setup);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        formLayout_3 = new QFormLayout();
        formLayout_3->setSpacing(6);
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        label_23 = new QLabel(grp_box_setup);
        label_23->setObjectName(QStringLiteral("label_23"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_23);

        rad_btn_Master = new QRadioButton(grp_box_setup);
        rad_btn_Master->setObjectName(QStringLiteral("rad_btn_Master"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, rad_btn_Master);

        rad_btn_Monitor = new QRadioButton(grp_box_setup);
        rad_btn_Monitor->setObjectName(QStringLiteral("rad_btn_Monitor"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, rad_btn_Monitor);


        verticalLayout->addLayout(formLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        btn_start_listen = new QPushButton(grp_box_setup);
        btn_start_listen->setObjectName(QStringLiteral("btn_start_listen"));

        verticalLayout_2->addWidget(btn_start_listen);

        btn_start_program = new QPushButton(grp_box_setup);
        btn_start_program->setObjectName(QStringLiteral("btn_start_program"));

        verticalLayout_2->addWidget(btn_start_program);

        btn_start_ips = new QPushButton(grp_box_setup);
        btn_start_ips->setObjectName(QStringLiteral("btn_start_ips"));

        verticalLayout_2->addWidget(btn_start_ips);

        btn_stop_listeners = new QPushButton(grp_box_setup);
        btn_stop_listeners->setObjectName(QStringLiteral("btn_stop_listeners"));
        btn_stop_listeners->setEnabled(true);

        verticalLayout_2->addWidget(btn_stop_listeners);


        verticalLayout->addLayout(verticalLayout_2);

        grp_box_messages = new QGroupBox(tab);
        grp_box_messages->setObjectName(QStringLiteral("grp_box_messages"));
        grp_box_messages->setGeometry(QRect(50, 380, 421, 281));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush2(QColor(216, 216, 216, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush2);
        grp_box_messages->setPalette(palette1);
        txt_messages_out = new QTextBrowser(grp_box_messages);
        txt_messages_out->setObjectName(QStringLiteral("txt_messages_out"));
        txt_messages_out->setGeometry(QRect(0, 30, 411, 241));
        txt_messages_out->setAcceptDrops(false);
        grp_box_telemetry = new QGroupBox(tab);
        grp_box_telemetry->setObjectName(QStringLiteral("grp_box_telemetry"));
        grp_box_telemetry->setGeometry(QRect(490, 390, 891, 311));
        gridLayoutWidget = new QWidget(grp_box_telemetry);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(9, 29, 238, 316));
        gridLayout_2 = new QGridLayout(gridLayoutWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        grp_box_cmd_count = new QGroupBox(gridLayoutWidget);
        grp_box_cmd_count->setObjectName(QStringLiteral("grp_box_cmd_count"));
        gridLayout_3 = new QGridLayout(grp_box_cmd_count);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label_3 = new QLabel(grp_box_cmd_count);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        label = new QLabel(grp_box_cmd_count);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label);

        label_4 = new QLabel(grp_box_cmd_count);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        label_6 = new QLabel(grp_box_cmd_count);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout->setWidget(11, QFormLayout::LabelRole, label_6);

        label_5 = new QLabel(grp_box_cmd_count);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_5);

        label_7 = new QLabel(grp_box_cmd_count);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_7);

        label_8 = new QLabel(grp_box_cmd_count);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_8);

        label_15 = new QLabel(grp_box_cmd_count);
        label_15->setObjectName(QStringLiteral("label_15"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_15);

        label_16 = new QLabel(grp_box_cmd_count);
        label_16->setObjectName(QStringLiteral("label_16"));

        formLayout->setWidget(10, QFormLayout::LabelRole, label_16);

        label_18 = new QLabel(grp_box_cmd_count);
        label_18->setObjectName(QStringLiteral("label_18"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_18);

        label_19 = new QLabel(grp_box_cmd_count);
        label_19->setObjectName(QStringLiteral("label_19"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_19);

        lbl_val_telecmd_pkt_cnt = new QLabel(grp_box_cmd_count);
        lbl_val_telecmd_pkt_cnt->setObjectName(QStringLiteral("lbl_val_telecmd_pkt_cnt"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lbl_val_telecmd_pkt_cnt);

        lbl_inv_telecmd_pkt_cnt = new QLabel(grp_box_cmd_count);
        lbl_inv_telecmd_pkt_cnt->setObjectName(QStringLiteral("lbl_inv_telecmd_pkt_cnt"));

        formLayout->setWidget(2, QFormLayout::FieldRole, lbl_inv_telecmd_pkt_cnt);

        lbl_val_cmd_cnt = new QLabel(grp_box_cmd_count);
        lbl_val_cmd_cnt->setObjectName(QStringLiteral("lbl_val_cmd_cnt"));

        formLayout->setWidget(3, QFormLayout::FieldRole, lbl_val_cmd_cnt);

        lbl_inv_cmd_cnt = new QLabel(grp_box_cmd_count);
        lbl_inv_cmd_cnt->setObjectName(QStringLiteral("lbl_inv_cmd_cnt"));

        formLayout->setWidget(4, QFormLayout::FieldRole, lbl_inv_cmd_cnt);

        lbl_cmd_exec_suc_cnt = new QLabel(grp_box_cmd_count);
        lbl_cmd_exec_suc_cnt->setObjectName(QStringLiteral("lbl_cmd_exec_suc_cnt"));

        formLayout->setWidget(5, QFormLayout::FieldRole, lbl_cmd_exec_suc_cnt);

        lbl_cmd_exec_err_cnt = new QLabel(grp_box_cmd_count);
        lbl_cmd_exec_err_cnt->setObjectName(QStringLiteral("lbl_cmd_exec_err_cnt"));

        formLayout->setWidget(6, QFormLayout::FieldRole, lbl_cmd_exec_err_cnt);

        lbl_tlm_pkt_xfr_frm_seq_cnt = new QLabel(grp_box_cmd_count);
        lbl_tlm_pkt_xfr_frm_seq_cnt->setObjectName(QStringLiteral("lbl_tlm_pkt_xfr_frm_seq_cnt"));

        formLayout->setWidget(7, QFormLayout::FieldRole, lbl_tlm_pkt_xfr_frm_seq_cnt);

        lbl_next_atc_tm_str = new QLabel(grp_box_cmd_count);
        lbl_next_atc_tm_str->setObjectName(QStringLiteral("lbl_next_atc_tm_str"));

        formLayout->setWidget(8, QFormLayout::FieldRole, lbl_next_atc_tm_str);

        lbl_pbk_prog_flg = new QLabel(grp_box_cmd_count);
        lbl_pbk_prog_flg->setObjectName(QStringLiteral("lbl_pbk_prog_flg"));

        formLayout->setWidget(9, QFormLayout::FieldRole, lbl_pbk_prog_flg);

        label_20 = new QLabel(grp_box_cmd_count);
        label_20->setObjectName(QStringLiteral("label_20"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_20);

        lbl_rx_cmd_pkt_cnt = new QLabel(grp_box_cmd_count);
        lbl_rx_cmd_pkt_cnt->setObjectName(QStringLiteral("lbl_rx_cmd_pkt_cnt"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lbl_rx_cmd_pkt_cnt);


        gridLayout_3->addLayout(formLayout, 0, 0, 1, 1);


        gridLayout_2->addWidget(grp_box_cmd_count, 0, 0, 1, 1);

        lbl_link_state = new QLabel(grp_box_telemetry);
        lbl_link_state->setObjectName(QStringLiteral("lbl_link_state"));
        lbl_link_state->setGeometry(QRect(250, 0, 141, 17));
        QPalette palette2;
        QBrush brush3(QColor(0, 0, 0, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush3);
        QBrush brush4(QColor(25, 255, 0, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush4);
        QBrush brush5(QColor(140, 255, 127, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Light, brush5);
        QBrush brush6(QColor(82, 255, 63, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Midlight, brush6);
        QBrush brush7(QColor(12, 127, 0, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Dark, brush7);
        QBrush brush8(QColor(16, 170, 0, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Mid, brush8);
        QBrush brush9(QColor(0, 255, 0, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush9);
        palette2.setBrush(QPalette::Active, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush3);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush4);
        palette2.setBrush(QPalette::Active, QPalette::Shadow, brush3);
        palette2.setBrush(QPalette::Active, QPalette::AlternateBase, brush5);
        QBrush brush10(QColor(255, 255, 220, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette2.setBrush(QPalette::Active, QPalette::ToolTipBase, brush10);
        palette2.setBrush(QPalette::Active, QPalette::ToolTipText, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::Light, brush5);
        palette2.setBrush(QPalette::Inactive, QPalette::Midlight, brush6);
        palette2.setBrush(QPalette::Inactive, QPalette::Dark, brush7);
        palette2.setBrush(QPalette::Inactive, QPalette::Mid, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush9);
        palette2.setBrush(QPalette::Inactive, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush1);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush4);
        palette2.setBrush(QPalette::Inactive, QPalette::Shadow, brush3);
        palette2.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush5);
        palette2.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush10);
        palette2.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Light, brush5);
        palette2.setBrush(QPalette::Disabled, QPalette::Midlight, brush6);
        palette2.setBrush(QPalette::Disabled, QPalette::Dark, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Mid, brush8);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::BrightText, brush1);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Shadow, brush3);
        palette2.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush10);
        palette2.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush3);
        lbl_link_state->setPalette(palette2);
        label_25 = new QLabel(grp_box_telemetry);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setGeometry(QRect(180, 0, 71, 17));
        grp_box_housekeeping = new QGroupBox(grp_box_telemetry);
        grp_box_housekeeping->setObjectName(QStringLiteral("grp_box_housekeeping"));
        grp_box_housekeeping->setGeometry(QRect(270, 30, 311, 249));
        formLayoutWidget_2 = new QWidget(grp_box_housekeeping);
        formLayoutWidget_2->setObjectName(QStringLiteral("formLayoutWidget_2"));
        formLayoutWidget_2->setGeometry(QRect(0, 20, 291, 251));
        formLayout_2 = new QFormLayout(formLayoutWidget_2);
        formLayout_2->setSpacing(6);
        formLayout_2->setContentsMargins(11, 11, 11, 11);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        label_21 = new QLabel(formLayoutWidget_2);
        label_21->setObjectName(QStringLiteral("label_21"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_21);

        lbl_acq_img_cnt = new QLabel(formLayoutWidget_2);
        lbl_acq_img_cnt->setObjectName(QStringLiteral("lbl_acq_img_cnt"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, lbl_acq_img_cnt);

        label_9 = new QLabel(formLayoutWidget_2);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_9);

        lbl_img_acq_prog_flag = new QLabel(formLayoutWidget_2);
        lbl_img_acq_prog_flag->setObjectName(QStringLiteral("lbl_img_acq_prog_flag"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, lbl_img_acq_prog_flag);

        label_10 = new QLabel(formLayoutWidget_2);
        label_10->setObjectName(QStringLiteral("label_10"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_10);

        lbl_ers_rly_swtch_state = new QLabel(formLayoutWidget_2);
        lbl_ers_rly_swtch_state->setObjectName(QStringLiteral("lbl_ers_rly_swtch_state"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, lbl_ers_rly_swtch_state);

        label_11 = new QLabel(formLayoutWidget_2);
        label_11->setObjectName(QStringLiteral("label_11"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_11);

        lbl_mdq_scan_state = new QLabel(formLayoutWidget_2);
        lbl_mdq_scan_state->setObjectName(QStringLiteral("lbl_mdq_scan_state"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, lbl_mdq_scan_state);

        label_13 = new QLabel(formLayoutWidget_2);
        label_13->setObjectName(QStringLiteral("label_13"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_13);

        lbl_flt_tbl_mode = new QLabel(formLayoutWidget_2);
        lbl_flt_tbl_mode->setObjectName(QStringLiteral("lbl_flt_tbl_mode"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, lbl_flt_tbl_mode);

        label_12 = new QLabel(formLayoutWidget_2);
        label_12->setObjectName(QStringLiteral("label_12"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_12);

        lbl_img_accpt_cnt = new QLabel(formLayoutWidget_2);
        lbl_img_accpt_cnt->setObjectName(QStringLiteral("lbl_img_accpt_cnt"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, lbl_img_accpt_cnt);

        label_14 = new QLabel(formLayoutWidget_2);
        label_14->setObjectName(QStringLiteral("label_14"));

        formLayout_2->setWidget(6, QFormLayout::LabelRole, label_14);

        lbl_img_rej_cnt = new QLabel(formLayoutWidget_2);
        lbl_img_rej_cnt->setObjectName(QStringLiteral("lbl_img_rej_cnt"));

        formLayout_2->setWidget(6, QFormLayout::FieldRole, lbl_img_rej_cnt);

        label_2 = new QLabel(formLayoutWidget_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout_2->setWidget(7, QFormLayout::LabelRole, label_2);

        lbl_next_img_acq_tm_str = new QLabel(formLayoutWidget_2);
        lbl_next_img_acq_tm_str->setObjectName(QStringLiteral("lbl_next_img_acq_tm_str"));

        formLayout_2->setWidget(7, QFormLayout::FieldRole, lbl_next_img_acq_tm_str);

        label_17 = new QLabel(formLayoutWidget_2);
        label_17->setObjectName(QStringLiteral("label_17"));

        formLayout_2->setWidget(9, QFormLayout::LabelRole, label_17);

        label_22 = new QLabel(formLayoutWidget_2);
        label_22->setObjectName(QStringLiteral("label_22"));

        formLayout_2->setWidget(8, QFormLayout::LabelRole, label_22);

        lbl_sys_tm_str = new QLabel(formLayoutWidget_2);
        lbl_sys_tm_str->setObjectName(QStringLiteral("lbl_sys_tm_str"));

        formLayout_2->setWidget(8, QFormLayout::FieldRole, lbl_sys_tm_str);

        groupBox = new QGroupBox(grp_box_telemetry);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(580, 30, 301, 231));
        formLayoutWidget = new QWidget(groupBox);
        formLayoutWidget->setObjectName(QStringLiteral("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(9, 29, 281, 191));
        formLayout_4 = new QFormLayout(formLayoutWidget);
        formLayout_4->setSpacing(6);
        formLayout_4->setContentsMargins(11, 11, 11, 11);
        formLayout_4->setObjectName(QStringLiteral("formLayout_4"));
        formLayout_4->setContentsMargins(0, 0, 0, 0);
        label_24 = new QLabel(formLayoutWidget);
        label_24->setObjectName(QStringLiteral("label_24"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_24);

        label_26 = new QLabel(formLayoutWidget);
        label_26->setObjectName(QStringLiteral("label_26"));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, label_26);

        label_27 = new QLabel(formLayoutWidget);
        label_27->setObjectName(QStringLiteral("label_27"));

        formLayout_4->setWidget(2, QFormLayout::LabelRole, label_27);

        lbl_magx = new QLabel(formLayoutWidget);
        lbl_magx->setObjectName(QStringLiteral("lbl_magx"));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, lbl_magx);

        lbl_magy = new QLabel(formLayoutWidget);
        lbl_magy->setObjectName(QStringLiteral("lbl_magy"));

        formLayout_4->setWidget(1, QFormLayout::FieldRole, lbl_magy);

        lbl_magz = new QLabel(formLayoutWidget);
        lbl_magz->setObjectName(QStringLiteral("lbl_magz"));

        formLayout_4->setWidget(2, QFormLayout::FieldRole, lbl_magz);

        btn_quit = new QPushButton(tab);
        btn_quit->setObjectName(QStringLiteral("btn_quit"));
        btn_quit->setGeometry(QRect(1390, 670, 87, 29));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        horizontalLayoutWidget = new QWidget(tab_2);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(10, 0, 1231, 591));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        tbl_cmd_list = new QTableWidget(horizontalLayoutWidget);
        tbl_cmd_list->setObjectName(QStringLiteral("tbl_cmd_list"));
        tbl_cmd_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl_cmd_list->setDragEnabled(true);
        tbl_cmd_list->setDragDropMode(QAbstractItemView::DragOnly);
        tbl_cmd_list->setDefaultDropAction(Qt::CopyAction);

        horizontalLayout->addWidget(tbl_cmd_list);

        tree_inactive_cmds = new QTreeWidget(horizontalLayoutWidget);
        tree_inactive_cmds->headerItem()->setText(0, QString());
        tree_inactive_cmds->setObjectName(QStringLiteral("tree_inactive_cmds"));
        QPalette palette3;
        QBrush brush11(QColor(227, 227, 227, 255));
        brush11.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::Base, brush11);
        palette3.setBrush(QPalette::Inactive, QPalette::Base, brush11);
        palette3.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        tree_inactive_cmds->setPalette(palette3);
        tree_inactive_cmds->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);
        tree_inactive_cmds->setDragEnabled(true);
        tree_inactive_cmds->setDragDropOverwriteMode(true);
        tree_inactive_cmds->setDragDropMode(QAbstractItemView::DropOnly);
        tree_inactive_cmds->setDefaultDropAction(Qt::MoveAction);
        tree_inactive_cmds->setAlternatingRowColors(true);
        tree_inactive_cmds->setSelectionMode(QAbstractItemView::ExtendedSelection);
        tree_inactive_cmds->setSelectionBehavior(QAbstractItemView::SelectItems);

        horizontalLayout->addWidget(tree_inactive_cmds);

        horizontalLayoutWidget_2 = new QWidget(tab_2);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(640, 600, 591, 51));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        txt_browser_add_cmd = new QLineEdit(horizontalLayoutWidget_2);
        txt_browser_add_cmd->setObjectName(QStringLiteral("txt_browser_add_cmd"));

        horizontalLayout_2->addWidget(txt_browser_add_cmd);

        btn_add_cmd = new QPushButton(horizontalLayoutWidget_2);
        btn_add_cmd->setObjectName(QStringLiteral("btn_add_cmd"));
        btn_add_cmd->setEnabled(true);

        horizontalLayout_2->addWidget(btn_add_cmd);

        btn_cmd_list_remove = new QPushButton(horizontalLayoutWidget_2);
        btn_cmd_list_remove->setObjectName(QStringLiteral("btn_cmd_list_remove"));

        horizontalLayout_2->addWidget(btn_cmd_list_remove);

        btn_set_cmd_list_active = new QPushButton(horizontalLayoutWidget_2);
        btn_set_cmd_list_active->setObjectName(QStringLiteral("btn_set_cmd_list_active"));
        btn_set_cmd_list_active->setEnabled(false);

        horizontalLayout_2->addWidget(btn_set_cmd_list_active);

        horizontalLayoutWidget_4 = new QWidget(tab_2);
        horizontalLayoutWidget_4->setObjectName(QStringLiteral("horizontalLayoutWidget_4"));
        horizontalLayoutWidget_4->setGeometry(QRect(6, 600, 621, 51));
        horizontalLayout_4 = new QHBoxLayout(horizontalLayoutWidget_4);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        btn_active_to_inactive = new QPushButton(horizontalLayoutWidget_4);
        btn_active_to_inactive->setObjectName(QStringLiteral("btn_active_to_inactive"));
        btn_active_to_inactive->setEnabled(false);

        horizontalLayout_4->addWidget(btn_active_to_inactive);

        btn_add_all = new QPushButton(horizontalLayoutWidget_4);
        btn_add_all->setObjectName(QStringLiteral("btn_add_all"));
        btn_add_all->setEnabled(true);

        horizontalLayout_4->addWidget(btn_add_all);

        btn_load_commands = new QPushButton(horizontalLayoutWidget_4);
        btn_load_commands->setObjectName(QStringLiteral("btn_load_commands"));

        horizontalLayout_4->addWidget(btn_load_commands);

        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1516, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "HEPCATS Ground Control", Q_NULLPTR));
        grp_box_cmd->setTitle(QApplication::translate("MainWindow", "Commanding", Q_NULLPTR));
        txt_prompt->setText(QString());
        txt_prompt->setPlaceholderText(QApplication::translate("MainWindow", "PROMPT>", Q_NULLPTR));
        btn_enter->setText(QApplication::translate("MainWindow", "Send", Q_NULLPTR));
        txt_cmd_add_cmd->setText(QString());
        txt_cmd_add_cmd->setPlaceholderText(QApplication::translate("MainWindow", "New Command Mnemonic", Q_NULLPTR));
        btn_tab_cmd_add_cmd->setText(QApplication::translate("MainWindow", "Add to List", Q_NULLPTR));
        cmd_txt_messages->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", Q_NULLPTR));
        QTreeWidgetItem *___qtreewidgetitem = tree_active_cmds->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("MainWindow", "HEPCATS Command List", Q_NULLPTR));

        const bool __sortingEnabled = tree_active_cmds->isSortingEnabled();
        tree_active_cmds->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = tree_active_cmds->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "C&DH", Q_NULLPTR));
        QTreeWidgetItem *___qtreewidgetitem2 = ___qtreewidgetitem1->child(0);
        ___qtreewidgetitem2->setText(0, QApplication::translate("MainWindow", "cmd HEPCATS NOOP with HOLD 0", Q_NULLPTR));
        QTreeWidgetItem *___qtreewidgetitem3 = tree_active_cmds->topLevelItem(1);
        ___qtreewidgetitem3->setText(0, QApplication::translate("MainWindow", "Imaging", Q_NULLPTR));
        QTreeWidgetItem *___qtreewidgetitem4 = tree_active_cmds->topLevelItem(2);
        ___qtreewidgetitem4->setText(0, QApplication::translate("MainWindow", "Magnetometer", Q_NULLPTR));
        QTreeWidgetItem *___qtreewidgetitem5 = tree_active_cmds->topLevelItem(3);
        ___qtreewidgetitem5->setText(0, QApplication::translate("MainWindow", "IPS", Q_NULLPTR));
        tree_active_cmds->setSortingEnabled(__sortingEnabled);

        grp_box_setup->setTitle(QApplication::translate("MainWindow", "Setup: PANEL INOP", Q_NULLPTR));
        label_23->setText(QApplication::translate("MainWindow", "Contact Type:", Q_NULLPTR));
        rad_btn_Master->setText(QApplication::translate("MainWindow", "Master", Q_NULLPTR));
        rad_btn_Monitor->setText(QApplication::translate("MainWindow", "Monitor", Q_NULLPTR));
        btn_start_listen->setText(QApplication::translate("MainWindow", "Start Listeners", Q_NULLPTR));
        btn_start_program->setText(QApplication::translate("MainWindow", "Start Onboard FSW", Q_NULLPTR));
        btn_start_ips->setText(QApplication::translate("MainWindow", "Start IPS", Q_NULLPTR));
        btn_stop_listeners->setText(QApplication::translate("MainWindow", "Stop Listeners", Q_NULLPTR));
        grp_box_messages->setTitle(QApplication::translate("MainWindow", "Messages", Q_NULLPTR));
        txt_messages_out->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Noto Sans'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", Q_NULLPTR));
        grp_box_telemetry->setTitle(QApplication::translate("MainWindow", "Telemetry: ", Q_NULLPTR));
        grp_box_cmd_count->setTitle(QApplication::translate("MainWindow", "Command Counts", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "Valid CMD PKT:", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Invalid CMD PKT:", Q_NULLPTR));
        label_4->setText(QApplication::translate("MainWindow", "Valid Commands:", Q_NULLPTR));
        label_6->setText(QString());
        label_5->setText(QApplication::translate("MainWindow", "Invalid Commands:", Q_NULLPTR));
        label_7->setText(QApplication::translate("MainWindow", "OK Execution", Q_NULLPTR));
        label_8->setText(QApplication::translate("MainWindow", "ERR Execution", Q_NULLPTR));
        label_15->setText(QApplication::translate("MainWindow", "Packet Seq Count:", Q_NULLPTR));
        label_16->setText(QString());
        label_18->setText(QApplication::translate("MainWindow", "Next ATC Time:", Q_NULLPTR));
        label_19->setText(QApplication::translate("MainWindow", "Playback State:", Q_NULLPTR));
        lbl_val_telecmd_pkt_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_inv_telecmd_pkt_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_val_cmd_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_inv_cmd_cnt->setText(QApplication::translate("MainWindow", "NO TLM ", Q_NULLPTR));
        lbl_cmd_exec_suc_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_cmd_exec_err_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_tlm_pkt_xfr_frm_seq_cnt->setText(QApplication::translate("MainWindow", "NO TLM ", Q_NULLPTR));
        lbl_next_atc_tm_str->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_pbk_prog_flg->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_20->setText(QApplication::translate("MainWindow", "Rx CMD PKT Count", Q_NULLPTR));
        lbl_rx_cmd_pkt_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_link_state->setText(QApplication::translate("MainWindow", "NO LINK", Q_NULLPTR));
        label_25->setText(QApplication::translate("MainWindow", "Link State:", Q_NULLPTR));
        grp_box_housekeeping->setTitle(QApplication::translate("MainWindow", "Housekeeping", Q_NULLPTR));
        label_21->setText(QApplication::translate("MainWindow", "Image Acquisition Count:", Q_NULLPTR));
        lbl_acq_img_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_9->setText(QApplication::translate("MainWindow", "Camera State:", Q_NULLPTR));
        lbl_img_acq_prog_flag->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_10->setText(QApplication::translate("MainWindow", "ERS Relay State", Q_NULLPTR));
        lbl_ers_rly_swtch_state->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_11->setText(QApplication::translate("MainWindow", "Mag State:", Q_NULLPTR));
        lbl_mdq_scan_state->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_13->setText(QApplication::translate("MainWindow", "Flight Mode:", Q_NULLPTR));
        lbl_flt_tbl_mode->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_12->setText(QApplication::translate("MainWindow", "Accepted Image Count", Q_NULLPTR));
        lbl_img_accpt_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_14->setText(QApplication::translate("MainWindow", "Rejected Image Count", Q_NULLPTR));
        lbl_img_rej_cnt->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Next Image Acq Time:", Q_NULLPTR));
        lbl_next_img_acq_tm_str->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        label_17->setText(QString());
        label_22->setText(QApplication::translate("MainWindow", "System Time", Q_NULLPTR));
        lbl_sys_tm_str->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("MainWindow", "Magnetometer", Q_NULLPTR));
        label_24->setText(QApplication::translate("MainWindow", "X Magnitude:", Q_NULLPTR));
        label_26->setText(QApplication::translate("MainWindow", "Y Magnitude:", Q_NULLPTR));
        label_27->setText(QApplication::translate("MainWindow", "Z Magnitude:", Q_NULLPTR));
        lbl_magx->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_magy->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        lbl_magz->setText(QApplication::translate("MainWindow", "NO TLM", Q_NULLPTR));
        btn_quit->setText(QApplication::translate("MainWindow", "Quit", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Commanding and Telemetry", Q_NULLPTR));
        txt_browser_add_cmd->setText(QString());
        txt_browser_add_cmd->setPlaceholderText(QApplication::translate("MainWindow", "Custom command mnemonic", Q_NULLPTR));
        btn_add_cmd->setText(QApplication::translate("MainWindow", "Add", Q_NULLPTR));
        btn_cmd_list_remove->setText(QApplication::translate("MainWindow", "Remove Item", Q_NULLPTR));
        btn_set_cmd_list_active->setText(QApplication::translate("MainWindow", "Set List as Active (INOP)", Q_NULLPTR));
        btn_active_to_inactive->setText(QApplication::translate("MainWindow", "INOP Get Active List", Q_NULLPTR));
        btn_add_all->setText(QApplication::translate("MainWindow", "Add All to List", Q_NULLPTR));
        btn_load_commands->setText(QApplication::translate("MainWindow", "Load Command List from Disk", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Command List Browser", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
