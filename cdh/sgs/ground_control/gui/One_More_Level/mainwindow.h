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
    void print_to_msgs();
    void add_custom_to_active();
    void startfsw();
    void startips();
    void ipsNotifyLog();
    void print_to_mag();
    void print_to_fsw();
    void print_to_img();


private:
    Ui::MainWindow *ui;
    //Make pointer for telemetry listener
    QProcess *listeners;
    QProcess *tlm_reader;
    QProcess *ipsNotify;
    QProcess *mag_reader;
    QProcess *fsw;
    QProcess *img_reader;



};

#endif // MAINWINDOW_H
