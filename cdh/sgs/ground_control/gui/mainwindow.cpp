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
#include "QTime"
#include "QProcess"
#include "iostream"
#include "QDebug"
#include "QFileDialog"
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
    connect(ui->btn_add_all,SIGNAL(clicked(bool)),this,SLOT(add_all_to_inactive()));

    //Command List Tab Signals
    connect(ui->btn_load_commands,SIGNAL(clicked(bool)),this,SLOT(load_command_list()));
    connect(ui->btn_cmd_list_remove, SIGNAL(clicked(bool)),this,SLOT(remove_cmd_item()));
    connect(ui->btn_add_cmd,SIGNAL(clicked(bool)),this,SLOT(add_custom_to_inactive()));


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
    QString command_to_send="./../../../../ground_control/backend/bin/gc_prompt " + command;
    QProcess commanding;
            commanding.start(command_to_send);
            commanding.waitForFinished(-1);
            QString stdout=commanding.readAllStandardOutput();
            QString stderr=commanding.readAllStandardError();

    //Display the command in the prompt window.

    QString prompt_out=cmd_time+" PROMPT: "+command;
    ui->cmd_txt_messages->append(prompt_out);
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
       QString gc_listen_cmd="./../../../../ground_control/backend/bin/test_tlm.sh";
       tlm_reader=new QProcess(this);
       tlm_reader->setProcessChannelMode(QProcess::MergedChannels);
       connect(tlm_reader,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_telem()));
       connect(tlm_reader,SIGNAL(readyReadStandardError()),this,SLOT(print_to_telem()));
       connect(tlm_reader,SIGNAL(readyRead()),this,SLOT(print_to_msgs()));
       tlm_reader->start(gc_listen_cmd);
       ui->txt_messages_out->append(current_time+": Listeners started.");
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

//Get STDOUT and STDERR output from telemetry process
output=tlm_reader->readAllStandardOutput();
Erroutput=tlm_reader->readAllStandardError();

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

ui->txt_messages_out->append("Received Telemety");

//
////Get telemetry values from output
rx_telecmd_pkt_cnt=output.section(',',0,0);
val_telecmd_pkt_cnt=output.section(',',1,1);
inv_telecmd_pkt_cnt=output.section(',',2,2);
val_cmd_cnt=output.section(',',3,3);
inv_cmd_cnt=output.section(',',4,4);
cmd_exec_suc_cnt=output.section(',',5,5);
cmd_exec_err_cnt=output.section(',',6,6);
tlm_pkt_xfr_frm_seq_cnt=output.section(',',7,7);
acq_img_cnt=output.section(',',8,8);
img_acq_prog_flag=output.section(',',9,9);
ers_rly_swtch_state=output.section(',',10,10);
mdq_scan_state=output.section(',',11,11);
flt_tbl_mode=output.section(',',12,12);
img_accpt_cnt=output.section(',',13,13);
img_rej_cnt=output.section(',',14,14);
next_img_acq_tm_str=output.section(',',15,15);
next_atc_tm_str=output.section(',',16,16);
pbk_prog_flag=output.section(',',17,17);
sys_tm_str=output.section(',',18,18);


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


}//END OF SLOT print_to_telem

void MainWindow::stop_listeners()
{
    //Placeholder function to kill telemetry listener
    QString current_time=QTime::currentTime().toString();
    tlm_reader->terminate();
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
    QString output=tlm_reader->readAllStandardOutput();
    QString errout=tlm_reader->readAllStandardError();
ui->txt_messages_out->append(output);
ui->txt_messages_out->append(errout);

}



///////////////////////////////////////////////////////////////////////////////////////////
/// Command Browser Tab SLOTS
///
///


void MainWindow::load_command_list()
{
      QString name=QFileDialog::getOpenFileName(this,tr("Open Command List"),"./../../../../testing/sgs_sim_ieu_test/sgs/",tr("HEPCATS Command Lists *.hcl"));
      QString data;
      QFile commandList(name);
      QStringList rows;
      QStringList rowData;
//Directly from the documentation doc.qt.io/qt-5/qfile
      if (!commandList.open(QIODevice::ReadOnly | QIODevice::Text))
          return;

      QTextStream in(&commandList);
      QStringList lines;
      QString CurrentLine;
      QStringList parsed_commands;
      QStringList titles;

      //Get all the lines from the CSV and put in a StringList
      while (!in.atEnd()) {
          CurrentLine = in.readLine();
          lines.append(CurrentLine);

      }
      //Get category titles and setup the table model
      titles=lines[0].split(",");
      //Setup table dimensions
      ui->tbl_cmd_list->setRowCount(lines.size());
      ui->tbl_cmd_list->setColumnCount(titles.size());

      for (int i=0; i<titles.size(); i++)
      {
          ui->tbl_cmd_list->setItem(0,i, new QTableWidgetItem(titles[0,i]));
      }

      for (int i=1; i < lines.size(); i ++) //Iterate through each saved line, putting contents into the table, initiate i at ONE because titles are defined at ZERO
      {
          parsed_commands=lines[i].split(","); //
          //Populate the table with parsed commands
          for (int x=0; x<titles.size();x++)
          {
              ui->tbl_cmd_list->setItem(i,x, new QTableWidgetItem(parsed_commands[0,x]));
          }
      }

}

void MainWindow::remove_cmd_item()
{
    delete ui->tree_inactive_cmds->currentItem();

}

void MainWindow::set_cmd_list_active()
{
/*
 *TODO
 * Take items from tree_inactive_cmds and copy to  tree_active_cmds
*/


}

void MainWindow::add_custom_to_inactive() //Adds the text in the inactive custom command box to the inactive command list.
{
//Get the custom command string
    QString custom_cmd=ui->txt_browser_add_cmd->text();
    QTreeWidgetItem *command = new QTreeWidgetItem();
    command->setText(0,custom_cmd);
    QTreeWidgetItem *location=ui->tree_inactive_cmds->currentItem();
    location->insertChild(0,command);
}

void MainWindow::add_custom_to_active() //Adds the text in the active custom command box to the active command list
{
    QString custom_cmd=ui->txt_cmd_add_cmd->text();
    QTreeWidgetItem *cmd = new QTreeWidgetItem();
    cmd->setText(0,custom_cmd);
    QTreeWidgetItem *location=ui->tree_active_cmds->currentItem();
    location->insertChild(0,cmd);
}

void MainWindow::add_all_to_inactive() //Takes all commands from loaded CSV and puts them in the inactive command list
{
    //Need to iterate through each command category, for now only three:
    for (int x=0; x<3; x++)
    {
        QString title=ui->tbl_cmd_list->item(0,x)->text();
        QTreeWidgetItem *tbl_title= new QTreeWidgetItem();
        tbl_title->setText(0,title);
        ui->tree_inactive_cmds->addTopLevelItem(tbl_title);

        //Get the number of items in the command category
        int num_items=ui->tbl_cmd_list->rowCount();
        for (int y=1; y<num_items;y++)
        {
            QString command_to_add=ui->tbl_cmd_list->item(y,x)->text();
            QTreeWidgetItem *command_item=new QTreeWidgetItem();
            command_item->setText(0,command_to_add);
            tbl_title->addChild(command_item);
        }

    }

}

void MainWindow::active_to_inactive_list() //Takes current active command list, overwrites the current inactive list
{

}













