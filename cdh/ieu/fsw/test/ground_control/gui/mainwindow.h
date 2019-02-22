#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QtGui>
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();
private slots:
    //Delare all slots to be used in mainwindow.cpp
    void cmd_return_pressed();
//    void cmd_send_btn_pressed();
    void on_btn_quit_clicked();
    void start_listeners();
    void print_to_telem();
    void stop_listeners();
    void link_master_checked();
    void link_monitor_checked();
    void load_command_list();
    void remove_cmd_item();
    void set_cmd_list_active();
    void add_custom_to_inactive();
    void add_custom_to_active();
    void add_all_to_inactive();
    void active_to_inactive_list();
    void sim_port_open_error();
    void print_to_msgs();


private:
    Ui::MainWindow *ui;
    //Make pointer for telemetry listener
    QProcess *listeners;
        //Pointers for sim connection processes
    QProcess *sim_port_setup_listener; //listener for sim port setup
    QProcess *sim_reader;
    QProcess *sim_tlm_loop;


};

#endif // MAINWINDOW_H
