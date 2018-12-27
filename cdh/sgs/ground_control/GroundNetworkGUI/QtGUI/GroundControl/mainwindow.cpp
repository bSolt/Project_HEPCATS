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
    connect(ui->btn_enter, SIGNAL(clicked(bool)),this,SLOT(cmd_return_pressed()));
    connect(ui->txt_prompt,SIGNAL(returnPressed()),this, SLOT(cmd_return_pressed()));
    connect(ui->btn_start_listen,SIGNAL(clicked(bool)),this,SLOT(start_listeners()));
    connect(ui->rad_btn_Master,SIGNAL(clicked(bool)),this,SLOT(link_master_checked()));
    connect(ui->rad_btn_Monitor,SIGNAL(clicked(bool)),this,SLOT(link_monitor_checked()));

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cmd_return_pressed() //Sends cmd_prompt string to command interpreter
{

    //Check to see if in commanding state
    if (ui->rad_btn_Master->isChecked())
    {
    //Takes input from prompt, displays it in the prompt window and sends it to the command interpreter
    //Get current sent command count
    QString cc_senttxt=ui->lbl_cmd_sent_count->text();
    int cc_sent=cc_senttxt.toInt();

    //Get the user's command
    ui->cmd_txt_messages->setTextColor(Qt::black);
    QString command=ui->txt_prompt->text();

    //Call ben's command interpreter
    QString command_to_send="./../../../../testing/sgs_sim_ieu_test/sgs/gc_prompt " + command;
    QProcess commanding;
            commanding.start(command_to_send);
            commanding.waitForFinished(-1);
            QString stdout=commanding.readAllStandardOutput();
            QString stderr=commanding.readAllStandardError();

    //Display the command in the prompt window.
    QString cmd_time=QTime::currentTime().toString();
    QString prompt_out=cmd_time+" PROMPT: "+command;
    ui->cmd_txt_messages->append(prompt_out);

    //Get execution status from interpreter
    QString rtn_time=QTime::currentTime().toString();

    //Check for commanding error
    bool is_cmd_error;
    is_cmd_error=stdout.contains("ERROR");
    if (is_cmd_error) //check if a commanding error exists
    {
        //If there is a command interpreter error
    ui->cmd_txt_messages->setTextColor(Qt::red);
    ui->cmd_txt_messages->append(rtn_time+": "+stdout); //update prompt with red text
    ui->cmd_txt_messages->setTextColor(Qt::black);
    }
    else
    {
        //if the command is valid or the interpreter returned no error
        ui->cmd_txt_messages->append(stdout); //update prompt with black text
        //Update command sent count
        cc_sent=cc_sent++;
        ui->lbl_cmd_sent_count->setText(QString::number(cc_sent));
    }

    //Clear the input line:
    ui->txt_prompt->clear();
    }//END OF if rad_btn_master->isChecked

    else //skip the interpreter since link is not set to master
    {
        QString current_time=QTime::currentTime().toString();
        //Print error in messages window
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append(current_time+": "+"COMMAND NOT SENT: LINK NOT MASTER");
        ui->txt_messages_out->setTextColor(Qt::black);

        //Print error in prompt window too
        ui->cmd_txt_messages->setTextColor(Qt::red);
        ui->cmd_txt_messages->append(current_time+": "+"<ERROR> LINK NOT MASTER: COMMAND NOT SENT"); //update prompt with red text
        ui->cmd_txt_messages->setTextColor(Qt::black);

    }




}


void MainWindow::on_btn_quit_clicked() //Closes main window
{
    close();
}

void MainWindow::start_listeners() //Spawns telemetry listeners
{


    //Set the process path
    QString commandtosend="./../../../../testing/sgs_sim_ieu_test/sgs/placeHoldTelem.sh"; //placeholder function, generates random telemetry packets to figure out updating GUI
    //Define listeners as a QProcess
    listeners=new QProcess(this);
    connect(listeners,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_telem())); //connect the data ready signal to the telem processor.
    listeners->start(commandtosend); //initiate the process asynchronously

}


//SLOT
//Called when telemetry has been written to the listener's STDOUT
void MainWindow::print_to_telem()
{
//Setup strings to hold telemetry output
QString output;
QString Erroutput;

//Get STDOUT and STDERR output from telemetry process
output=listeners->readAllStandardOutput();
Erroutput=listeners->readAllStandardError();
//Initialize telemetry holders
QString IEU_temp;
QString IEU_volt;
QString IMG_rate;
QString HAS_img;
QString MAG_rate;
QString MAG_packets;
QString cc_exetxt;
QString cc_acktxt;
QString cc_errtxt;

//Take the telemetry and seperate the values, comma delimited using QString Section
IEU_temp=output.section(',',0,0);
IEU_volt=output.section(',',1,1);
IMG_rate=output.section(',',2,2);
HAS_img=output.section(',',3,3);
MAG_rate=output.section(',',4,4);
MAG_packets=output.section(',',5,5);
cc_exetxt=output.section(',',6,6);
cc_acktxt=output.section(',',7,7);
cc_errtxt=output.section(',',8,8);

//Update the GUI's appropriate feilds
ui->lbl_IEU_temp->setText(IEU_temp);
ui->lbl_IEU_volt->setText(IEU_volt);
ui->lbl_IMG_rate->setText(IMG_rate);
ui->lbl_mag_rate->setText(MAG_rate);

//Check to see if there is a mag packet or Imaging packet


//Update telemetry counters
//Get the current telemetry packet count from GUI, this is a really bad way to do this, need to make a pointer or something
QString currentCount=ui->lbl_telem_rx->text();
int currentCountint=currentCount.toInt()+1;
ui->lbl_telem_rx->setText(QString::number(currentCountint));

//Update imaging counters
if (HAS_img.toInt()==1)
{
    QString currentCountImg=ui->lbl_IMG_rx->text();
int currentImg=currentCountImg.toInt()+1;
ui->lbl_IMG_rx->setText(QString::number(currentImg));
}

//Update MAG counter
if (MAG_packets.toInt()!=0)
{
    QString currentCountMag=ui->lbl_mag_rx->text();
int currentMag=currentCountMag.toInt()+MAG_packets.toInt();
ui->lbl_mag_rx->setText(QString::number(currentMag));
}

//Update Command Counts
ui->lbl_cmd_ack_count->setText(cc_acktxt);
ui->lbl_cmd_error_count->setText(cc_errtxt);
ui->lbl_cmd_exec_count->setText(cc_exetxt);

//Get command count descrepency (cc sent- cc ack) and update gui
QString cc_senttxt=ui->lbl_cmd_sent_count->text();
int cc_sent=cc_senttxt.toInt();
int cc_ack=cc_acktxt.toInt();
int cc_disc=cc_sent-cc_ack;

ui->lbl_disc->setText(QString::number(cc_disc));




}//END OF SLOT print_to_telem

void MainWindow::stop_listeners()
{
    //Placeholder function to kill telemetry listener
}

void MainWindow::link_master_checked() //Sets link state to active when link master is checked
{
   if (ui->rad_btn_Master->isChecked())
   {
    ui->lbl_link_state->setText("Active: Master");
   }
   else
   {
       ui->lbl_link_state->setText("NO LINK");
   }
}

void MainWindow::link_monitor_checked()
{
    if (ui->rad_btn_Monitor->isChecked())
    {
        ui->lbl_link_state->setText("Active: Monitor");
    }
    else
    {
        ui->lbl_link_state->setText("NO LINK");
    }
}


