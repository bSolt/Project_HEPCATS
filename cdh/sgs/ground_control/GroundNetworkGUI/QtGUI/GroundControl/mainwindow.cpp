#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdlib.h"
#include "QTime"
#include "QProcess"
#include "iostream"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Setup the window destroy
    connect(ui->btn_quit,SIGNAL(clicked(bool)),this,SLOT(on_btn_quit_clicked()));

    //Commanding Signals
    connect(ui->btn_enter, SIGNAL(clicked(bool)),this,SLOT(cmd_send_btn_pressed()));
    connect(ui->txt_prompt,SIGNAL(returnPressed()),this, SLOT(cmd_return_pressed()));
    connect(ui->btn_start_listen,SIGNAL(clicked(bool)),this,SLOT(start_listeners()));
//Spawn telemetry listener
    QProcess *telemetry=new QProcess();
    connect(telemetry,SIGNAL(readyReadStandardOutput()), this, SLOT(print_to_messages()));
    telemetry->start("sh testProcess.sh",NULL);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cmd_return_pressed() //Sends cmd_prompt string to command interpreter
{
    //Takes input from prompt, displays it in the prompt window and sends it to the command interpreter
    //Get the user's command
    QString command=ui->txt_prompt->text();

    //For now, put the user's command into a txt file till I know specific commands for ben's code
    //QString command_to_send="echo "+command+">testOutput"; //Suppressed to test cmd returns

    //Call ben's command interpreter
    QString command_to_send="./../../../../testing/sgs_sim_ieu_test/sgs/gc_prompt " + command;
    system(qPrintable(command_to_send));


    //Display the command in the prompt window.
    QString cmd_time=QTime::currentTime().toString();
    QString prompt_out=cmd_time+" PROMPT: "+command;

    ui->cmd_txt_messages->append(prompt_out);


    //Get the execution status from the command interpreter


    //Clear the input line:
    ui->txt_prompt->clear();
}

void MainWindow::cmd_send_btn_pressed() //Sends command selected to command interperter
{
    //TODO
    //Make the program not crash when no command is selected
    //Get the item current selected item
    QString command=ui->lst_commands->currentItem()->text();

    //For now, put the user's command into a txt file till I know specific commands for ben's code
    QString command_to_send="echo "+command+">testOutput"; //testing output



    //Display the command in the prompt window.
    QString cmd_time=QTime::currentTime().toString();
    QString prompt_out=cmd_time+" PROMPT: "+command;

    ui->cmd_txt_messages->append(prompt_out);

}




void MainWindow::on_btn_quit_clicked() //Closes main window
{
    close();
}

void MainWindow::start_listeners() //Spawns listeners to STDOUT
{


    //Setup a pointer to the parent process
    //Set the process path
    QString program="sh testProcess.sh";

    //Setup a pointer for the process and make a parent for the process
    QProcess *telemetry=new QProcess(this);

    connect(telemetry,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_messages()));

    // Start the process
    telemetry->start(program);

    telemetry->waitForFinished();
}
    //Setup a pointer to the parent process
//    QObject *parent;

//    //Set the process path
//    QString program="./testProcess.sh";

//    //Setup a pointer for the process and make a parent for the process
//    QProcess *telemetry=new QProcess(parent);

//    // Start the process
//    telemetry->start(program);
//    //Allocate places to put the STDOUT
//    QByteArray ba;
//    if(telemetry->waitForStarted(-1)) {
//        while(telemetry->waitForReadyRead(-1)){
//            ba += telemetry->readAllStandardOutput();
//        }
//    }
//char *telemetry_data=ba.data();
//ui->txt_messages_out->append(telemetry_data);

//}

void MainWindow::print_to_messages()
{
ui->txt_messages_out->append("Standing By:");
}


