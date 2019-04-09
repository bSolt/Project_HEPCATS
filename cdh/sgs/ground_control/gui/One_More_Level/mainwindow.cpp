/*
 * Created by Christopher Peercy
 * For CU Aerospace Engineering Senior Projects
 * Project HEPCATS
 *
 * Command and Telemetry GUI
 *
 * TODO:
 * Fix crash when adding custom command mnemonic if no selection has been made in tree_active_cmds
 *
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stdlib.h"
#include "stdio.h" //needed for writing telemetry to files
#include "QTime"
#include "QProcess" //Needed for telemetry listener and commanding
#include "iostream"
#include "QDebug"
#include "QFileDialog" //Load command lists from file
#include "QTextStream"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //SIGNALS HERE

    ui->setupUi(this);
    //Setup the window destroy
    connect(ui->btn_quit,SIGNAL(clicked(bool)),this,SLOT(on_btn_quit_clicked()));

    //Commanding and Telemetry Tab Signals
    connect(ui->btn_enter, SIGNAL(clicked(bool)),this,SLOT(cmd_return_pressed()));
    connect(ui->txt_prompt,SIGNAL(returnPressed()),this, SLOT(cmd_return_pressed()));
    connect(ui->btn_start_listen,SIGNAL(clicked(bool)),this,SLOT(start_listeners()));
    connect(ui->btn_stop_listeners,SIGNAL(clicked(bool)),this,SLOT(stop_listeners()));
    connect(ui->btn_tab_cmd_add_cmd,SIGNAL(clicked(bool)),this,SLOT(add_custom_to_active()));
    connect(ui->btn_start_program,SIGNAL(clicked(bool)),this,SLOT(startfsw()));
    connect(ui->btn_start_ips,SIGNAL(clicked(bool)),this,SLOT(startips()));


}//END MAINWINDOW

MainWindow::~MainWindow()
{
    delete ui;
}//END DESTROY MAINWINDOW

//SLOTS SLOTS SLOTS


void MainWindow::cmd_return_pressed() //Sends cmd_prompt string to command interpreter
{
/*
 * TODO WIP
 * Change from if to swtich statements for button selection.
 * Case 1 Master
 *      ACTUAL connection to SC
 * Case 2 Monitor
 *      ACTUAL connection to SC
 */

    int cmd_case;
    QString cmd_time=QTime::currentTime().toString();
    //Figure out what state is checked:
    if (ui->rad_btn_Master->isChecked())
    {
        cmd_case=1;
    }
    if (ui->rad_btn_Monitor->isChecked())
    {
        cmd_case=2;
    }

    switch(cmd_case)
    {
    //Check to see if in commanding states
    case (1): //MASTER CASE
    {
    //Takes input from prompt, displays it in the prompt window and sends it to the command interpreter
    //Get the user's command
    ui->cmd_txt_messages->setTextColor(Qt::black);
    QString command=ui->txt_prompt->text();

    //Call ben's command interpreter
    QString command_to_send="/mnt/1bc75f60-dc3a-49cb-bd0c-10cbfe5e30cb/HEPCATS_dev/project_hepcats/cdh/sgs/ground_control/backend/bin/gc_prompt " + command;
    QProcess commanding;
            //ui->cmd_txt_messages->append(command);
            //ui->cmd_txt_messages->append(command_to_send);
            commanding.start(command_to_send);
            commanding.waitForFinished(-1);
            QString stdout=commanding.readAllStandardOutput();
            QString stderr=commanding.readAllStandardError();
            QString cmderror=commanding.errorString();
           //Display the command in the prompt window.

    QString prompt_out=cmd_time+" PROMPT: "+command;
    ui->cmd_txt_messages->append(prompt_out);
    ui->cmd_txt_messages->append(cmderror); //uncomment to debug commanding errors
    QString rtn_time=QTime::currentTime().toString();


    //Check to see if  gc_prompt segfaulted:
    if (cmderror == 1){
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->cmd_txt_messages->setTextColor(Qt::red);
        ui->cmd_txt_messages->append(rtn_time+" <ERROR>: Command not trasmitted");
        ui->cmd_txt_messages->append(rtn_time+" <GC_PROMPT> ERROR: Segmentation fault");
        ui->txt_messages_out->append(rtn_time+" <ERROR> Command interpreter encounted segfault:");
        ui->txt_messages_out->append("Check path to command.csv is correct in interp_cmd_str.c and recompile gc_prompt_gui");
        ui->txt_messages_out->setTextColor(Qt::black);
        ui->cmd_txt_messages->setTextColor(Qt::black);
    }

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
        ui->cmd_txt_messages->append(stderr);
    }

    //Clear the input line:

        break;
} //END CASE 1 MASTER
    case (2): //MONITOR selected
    {
        QString current_time=QTime::currentTime().toString();
        //Print error in messages window
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append(current_time+": "+"<ERROR> COMMAND NOT SENT: CONTACT NOT MASTER");
        ui->txt_messages_out->setTextColor(Qt::black);

        //Print error in prompt window too
        ui->cmd_txt_messages->setTextColor(Qt::red);
        ui->cmd_txt_messages->append(current_time+": "+"<ERROR> CONTACT NOT MASTER: COMMAND NOT SENT"); //update prompt with red text
        ui->cmd_txt_messages->setTextColor(Qt::black);

    break;
} //END CASE 2 MONITOR
    default:
    {
        QString current_time=QTime::currentTime().toString();
        //Print error in messages window
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append(current_time+": "+"<ERROR> COMMAND NOT SENT: NO CONTACT");
        ui->txt_messages_out->setTextColor(Qt::black);

        //Print error in prompt window too
        ui->cmd_txt_messages->setTextColor(Qt::red);
        ui->cmd_txt_messages->append(current_time+": "+"<ERROR> NO CONTACT"); //update prompt with red text
        ui->cmd_txt_messages->setTextColor(Qt::black);
} //END CASE DEFAULT

    } //END SWTICH CASE
 ui->txt_prompt->clear();
}// END OF SLOT CMD_RETURN_PRESSED

void MainWindow::on_btn_quit_clicked() //Closes main window
{
    close();
}//END OF SLOT ON_BTN_QUIT_CLICKED

void MainWindow::start_listeners() //Spawns telemetry listeners
{

    int cmd_case=0;
    //Figure out what state is checked:
    if (ui->rad_btn_Master->isChecked() || ui->rad_btn_Monitor->isChecked())
    {
        cmd_case=1;
    }
    switch (cmd_case)
    {
    case 1: //Master or Monitor
    {
        QString current_time=QTime::currentTime().toString();
        //Start the gc_listener
        //HOUSEKEEPING TELEMETRY
        QString gc_listen_cmd="sh /mnt/1bc75f60-dc3a-49cb-bd0c-10cbfe5e30cb/HEPCATS_dev/project_hepcats/cdh/sgs/ground_control/backend/src/hacks/rcv_tlm_hack.sh";
        //system("pwd");
       tlm_reader=new QProcess(this);
       tlm_reader->setProcessChannelMode(QProcess::MergedChannels);
       connect(tlm_reader,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_telem()));
       connect(tlm_reader,SIGNAL(readyReadStandardError()),this,SLOT(print_to_telem()));
       //connect(tlm_reader,SIGNAL(readyRead()),this,SLOT(print_to_msgs()));
       tlm_reader->start(gc_listen_cmd);
       ui->txt_messages_out->append(current_time+":Housekeeping Listeners started.");

      QString mag_listen_cmd="sh /mnt/1bc75f60-dc3a-49cb-bd0c-10cbfe5e30cb/HEPCATS_dev/project_hepcats/cdh/sgs/ground_control/backend/src/hacks/rcv_mag_hack.sh";
      mag_reader=new QProcess(this);
      mag_reader->setProcessChannelMode(QProcess::MergedChannels);
      connect(mag_reader,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_mag()));
      connect(mag_reader,SIGNAL(readyReadStandardError()),this,SLOT(print_to_mag()));
      //connect(mag_reader,SIGNAL(readyRead()),this,SLOT(print_to_msgs()));
      mag_reader->start(mag_listen_cmd);
      ui->txt_messages_out->append(current_time+": Mag Listeners started.");

      QString img_listen_cmd="sh /mnt/1bc75f60-dc3a-49cb-bd0c-10cbfe5e30cb/HEPCATS_dev/project_hepcats/cdh/sgs/ground_control/backend/src/hacks/rcv_img_hack.sh";
      img_reader=new QProcess(this);
      img_reader->setProcessChannelMode(QProcess::MergedChannels);
      connect(img_reader,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_img()));
      connect(img_reader,SIGNAL(readyReadStandardError()),this,SLOT(print_to_img()));
      //connect(mag_reader,SIGNAL(readyRead()),this,SLOT(print_to_msgs()));
      img_reader->start(img_listen_cmd);
      ui->txt_messages_out->append(current_time+": Image Listeners started.");








       if (ui->rad_btn_Master->isChecked())
       ui->lbl_link_state->setText("MASTER");
       else
       ui->lbl_link_state->setText("MONITOR");
       break;
    }//END CASE MASTER

    default:
    {
        //Set default case for if no link type has been selected
        QString current_time=QTime::currentTime().toString();
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append(current_time+": Could not start listeners, link type not specified");
        ui->txt_messages_out->setTextColor(Qt::black);
        break;
    }
    } //END switch(cmd_case)
} //END SLOT start_listeners()


//SLOT
//Called when telemetry has been written to the listener's STDOUT
void MainWindow::print_to_telem()
{
//Setup strings to hold telemetry output
QString output;
QString Erroutput;
int tlm_type=0;


//Get STDOUT and STDERR output from telemetry process
output=tlm_reader->readAllStandardOutput();

//Determine if the telemetry is mag data or not, 1 for HK, 2 for Mag, 3 for bad packet.
if (output.contains("0x00"))
{
    tlm_type=1; //Packet is housekeeping
}
else if (output.contains("0xC8"))
{
    tlm_type=2; //Packet is mag
}
else if (output.contains("ERROR"))
{
    tlm_type=3;
}

switch (tlm_type)
{
case 1:
{
////Initialize telemetry holders
QString rx_telecmd_pkt_cnt;
QString val_telecmd_pkt_cnt;
QString inv_telecmd_pkt_cnt;
QString val_cmd_cnt;
QString inv_cmd_cnt;
QString cmd_exec_suc_cnt;
QString cmd_exec_err_cnt;
QString tlm_pkt_xfr_frm_seq_cnt;
QString acq_img_cnt;
QString img_acq_prog_flag;
QString ers_rly_swtch_state;
QString mdq_scan_state;
QString flt_tbl_mode;
QString img_accpt_cnt;
QString img_rej_cnt;
QString next_img_acq_tm_str;
QString next_atc_tm_str;
QString pbk_prog_flag;
QString sys_tm_str;
QString header;
QString ips_mdl_state;

//Save the telemetry to tlm_log
QString filename;
        filename="tlm_log.txt";
QFile tlm_log(filename);
if (tlm_log.open(QIODevice::Append))
{
    QTextStream telemetry(&tlm_log);
    telemetry << output << endl;
}

//ui->txt_messages_out->append("Received Telemety"); //uncomment to debug telemetry

//
////Get telemetry values from output
header=output.section(':',0,0);
rx_telecmd_pkt_cnt=output.section(',',1,1);
val_telecmd_pkt_cnt=output.section(',',2,2);
inv_telecmd_pkt_cnt=output.section(',',3,3);
val_cmd_cnt=output.section(',',4,4);
inv_cmd_cnt=output.section(',',5,5);
cmd_exec_suc_cnt=output.section(',',6,6);
cmd_exec_err_cnt=output.section(',',7,7);
tlm_pkt_xfr_frm_seq_cnt=output.section(',',8,8);
acq_img_cnt=output.section(',',9,9);
img_acq_prog_flag=output.section(',',10,10);
ers_rly_swtch_state=output.section(',',11,11);
mdq_scan_state=output.section(',',12,12);
flt_tbl_mode=output.section(',',13,13);
img_accpt_cnt=output.section(',',14,14);
img_rej_cnt=output.section(',',15,15);
next_img_acq_tm_str=output.section(',',16,16);
next_atc_tm_str=output.section(',',17,17);
pbk_prog_flag=output.section(',',18,18);
sys_tm_str=output.section(',',19,19);
ips_mdl_state=output.section(',',20,20);


////Update the GUI's appropriate fields
ui->lbl_rx_cmd_pkt_cnt->setText(rx_telecmd_pkt_cnt);
ui->lbl_val_telecmd_pkt_cnt->setText(val_telecmd_pkt_cnt);
ui->lbl_inv_telecmd_pkt_cnt->setText(inv_telecmd_pkt_cnt);
ui->lbl_val_cmd_cnt->setText(val_cmd_cnt);
ui->lbl_inv_cmd_cnt->setText(inv_cmd_cnt);
ui->lbl_cmd_exec_suc_cnt->setText(cmd_exec_suc_cnt);
ui->lbl_cmd_exec_err_cnt->setText(cmd_exec_err_cnt);
ui->lbl_tlm_pkt_xfr_frm_seq_cnt->setText(tlm_pkt_xfr_frm_seq_cnt);
ui->lbl_acq_img_cnt->setText(acq_img_cnt);
ui->lbl_img_acq_prog_flag->setText(img_acq_prog_flag);
ui->lbl_ers_rly_swtch_state->setText(ers_rly_swtch_state);
ui->lbl_mdq_scan_state->setText(mdq_scan_state);
ui->lbl_flt_tbl_mode->setText(flt_tbl_mode);
ui->lbl_img_accpt_cnt->setText(img_accpt_cnt);
ui->lbl_img_rej_cnt->setText(img_rej_cnt);
ui->lbl_next_img_acq_tm_str->setText(next_img_acq_tm_str);
ui->lbl_next_atc_tm_str->setText(next_atc_tm_str);
ui->lbl_pbk_prog_flg->setText(pbk_prog_flag);
ui->lbl_sys_tm_str->setText(sys_tm_str);
ui->lbl_ips_mdl_state->setText(ips_mdl_state);



break;
} //End case Housekeeping

case 2:
{
    //Write mag data to file
    QString filename;
            filename="mag_log.txt";
    QFile mag_log(filename);
    if (mag_log.open(QIODevice::Append))
    {
        QTextStream magdata(&mag_log);
        magdata << output << endl;
    }
   QString header=output.section(':',0,0);
   QString mag_x=output.section(',',1,1);
   QString mag_y=output.section(',',2,2);
   QString mag_z=output.section(',',3,3);

   ui->lbl_magx->setText(mag_x);
   ui->lbl_magy->setText(mag_y);
   ui->lbl_magz->setText(mag_z);
   break;
}// End case MAG Data

case (3): //rcv_tlm error
{
    //Write the error to tlm error file.
    QString filename;
            filename="tlm_err_log.txt";
    QFile err_log(filename);
    if (err_log.open(QIODevice::Append))
    {
        QTextStream errdata(&err_log);
        errdata << output << endl;
    }
 ui->txt_messages_out->setTextColor(Qt::red);
 ui->txt_messages_out->append(output);
 ui->txt_messages_out->append("Listeners FAILED. Contact not established.");
 ui->lbl_link_state->setText("FAILED");
 ui->txt_messages_out->setTextColor(Qt::black);
 break;
}


default: //packet may be corrupted
    {
        //ui->txt_messages_out->append("<TLM ERROR> APID not matched, packet may be corrupted");
        //Write mag data to file
        QString filename;
                filename="tlm_err_log.txt";
        QFile err_log(filename);
        if (err_log.open(QIODevice::Append))
        {
            QTextStream errdata(&err_log);
           // errdata << output << endl;
        }
        break;
    }//end case default

} //END SWITCH TLM_TYPE




}//END OF SLOT print_to_telem

void MainWindow::stop_listeners()
{
    //Placeholder function to kill telemetry listener
    QString current_time=QTime::currentTime().toString();
    tlm_reader->terminate();
    mag_reader->terminate();
    img_reader->terminate();
    ui->txt_messages_out->append(current_time+": "+"Signal to kill listeners sent");
    //Need to turn off auto exclusive due to Qt bug
    ui->rad_btn_Master->setAutoExclusive(false);
    ui->rad_btn_Monitor->setAutoExclusive(false);
    //Uncheck radiobuttons
    ui->rad_btn_Master->setChecked(false);
    ui->rad_btn_Monitor->setChecked(false);
    //Turn autoexclusive back on
    ui->rad_btn_Master->setAutoExclusive(true);
    ui->rad_btn_Monitor->setAutoExclusive(true);
    ui->lbl_link_state->setText("NO LINK");
}//END OF SLOT STOP LISTENERS

void MainWindow::print_to_msgs()
{
//Debug slot


}

void MainWindow::startfsw()
{



    QString start_fsw_cmd="sh /mnt/1bc75f60-dc3a-49cb-bd0c-10cbfe5e30cb/HEPCATS_dev/project_hepcats/cdh/sgs/ground_controlbackend/scripts/start_program.sh";
   fsw=new QProcess(this);
   fsw->setProcessChannelMode(QProcess::MergedChannels);
   connect(fsw,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_fsw()));
   connect(fsw,SIGNAL(readyReadStandardError()),this,SLOT(print_to_fsw()));
   //connect(tlm_reader,SIGNAL(readyRead()),this,SLOT(print_to_msgs()));
   fsw->start(start_fsw_cmd);
   ui->txt_messages_out->append("Starting fsw.");
    //ui->txt_messages_out->append(errorrtn); //uncomment to debug
}

void MainWindow::startips()
{
    QString ips_start_cmd="sh /mnt/1bc75f60-dc3a-49cb-bd0c-10cbfe5e30cb/HEPCATS_dev/project_hepcats/cdh/sgs/ground_control/backend/scripts/start_ips.sh";
    ipsNotify=new QProcess(this);
    ipsNotify->setProcessChannelMode(QProcess::MergedChannels);
    connect(ipsNotify,SIGNAL(readyReadStandardOutput()),this,SLOT(ipsNotifyLog()));
    connect(ipsNotify,SIGNAL(readyReadStandardError()),this,SLOT(ipsNotifyLog()));
    //connect(ipsNotify,SIGNAL(readyRead()),this,SLOT(print_to_msgs()));
    ipsNotify->start(ips_start_cmd);
    ui->txt_messages_out->append("IPS Starting. Monitor telemetry and IPS log.");
}

void MainWindow::ipsNotifyLog()
{
    QString output=ipsNotify->readAllStandardOutput();
    ui->txt_ips_log->append(output);
}

void MainWindow::print_to_mag()
{
    QString output=mag_reader->readAllStandardOutput();
    //Append the mag data to the log
    QString filename;
            filename="mag_tlm_log.txt";
    QFile mag_log(filename);
    if (mag_log.open(QIODevice::Append))
    {
        QTextStream filename(&mag_log);
        filename << output << endl;
    }
    QString magx, magy, magz;
    QString SingleLine;
    SingleLine=output.section(',',0,2);
    //ui->txt_messages_out->append(SingleLine);
    ui->lbl_magx->setText(SingleLine.section(',',0,0));
    ui->lbl_magy->setText(SingleLine.section(',',1,1));
    ui->lbl_magz->setText(SingleLine.section(',',2,2));
}

void MainWindow::print_to_fsw()
{
    QString output=fsw->readAllStandardOutput();
    ui->txt_fsw->append(output);
}

void MainWindow::print_to_img()
{
    QString numdown=ui->lbl_num_downlinked->text();
    int num;
    num=numdown.toInt();
    num=num+1;
    ui->lbl_num_downlinked->setText(QString::number(num));
}

void MainWindow::add_custom_to_active() //Adds the text in the active custom command box to the active command list
{
    QString custom_cmd=ui->txt_cmd_add_cmd->text();
    QTreeWidgetItem *cmd = new QTreeWidgetItem();
    cmd->setText(0,custom_cmd);
    QTreeWidgetItem *location;
    location=ui->tree_active_cmds->currentItem();
    if (location != 0x0){ //check to see if the user has selected a location
    location->insertChild(0,cmd);
    }
    else{
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append("Could not add custom mnemonic:");
        ui->txt_messages_out->append("You must select a subsystem or command set first.");
        ui->txt_messages_out->setTextColor(Qt::black);
    }
}







