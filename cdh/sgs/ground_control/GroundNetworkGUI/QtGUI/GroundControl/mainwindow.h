#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QtGui>

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
    void cmd_return_pressed();
//    void cmd_send_btn_pressed();
    void on_btn_quit_clicked();
    void start_listeners();
    void print_to_telem();
    void stop_listeners();
    void link_master_checked();
    void link_monitor_checked();

private:
    Ui::MainWindow *ui;
    //Make pointer for telemetry listener
    QProcess *listeners;


};

#endif // MAINWINDOW_H
