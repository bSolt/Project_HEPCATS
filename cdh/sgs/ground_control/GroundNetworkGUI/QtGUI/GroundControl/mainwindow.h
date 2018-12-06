#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>

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
    void cmd_send_btn_pressed();
    void on_btn_quit_clicked();
    void start_listeners();
    void print_to_messages();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
