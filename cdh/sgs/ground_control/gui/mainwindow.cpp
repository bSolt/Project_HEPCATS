/*
 * Created by Christopher Peercy
 * For CU Aerospace Engineering Senior Projects
 * Project HEPCATS
 *
 * Command and Telemetry GUI
 *
 * TODO:
 * Setup cmd_return_pressed cases to reflect actual conditions.
 * Setup start_listeners cases to accept sim connections
 * Setup start_lsiteners cases to send and listen for actual telemetry
 *
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
    connect(ui->rad_btn_Master,SIGNAL(clicked(bool)),this,SLOT(link_master_checked()));
    connect(ui->rad_btn_Monitor,SIGNAL(clicked(bool)),this,SLOT(link_monitor_checked()));
    connect(ui->btn_stop_listeners,SIGNAL(clicked(bool)),this,SLOT(stop_listeners()));
    connect(ui->btn_tab_cmd_add_cmd,SIGNAL(clicked(bool)),this,SLOT(add_custom_to_active()));
    connect(ui->btn_add_all,SIGNAL(clicked(bool)),this,SLOT(add_all_to_inactive()));
    //Command List Tab Signals
    connect(ui->btn_load_commands,SIGNAL(clicked(bool)),this,SLOT(load_command_list()));
    connect(ui->btn_cmd_list_remove, SIGNAL(clicked(bool)),this,SLOT(remove_cmd_item()));
    connect(ui->btn_add_cmd,SIGNAL(clicked(bool)),this,SLOT(add_custom_to_inactive()));


}
MainWindow::~MainWindow()
{
    delete ui;
}

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
 * Case 3 sim
 *      Sim connection, use sim telemetry from sim_ieu_send_tlm_packet in testing folder
 */

    int cmd_case;
    //Figure out what state is checked:
    if (ui->rad_btn_Master->isChecked())
    {
        cmd_case=1;
    }
    if (ui->rad_btn_Monitor->isChecked())
    {
        cmd_case=2;
    }
    if (ui->rad_btn_Sim->isChecked())
    {
        cmd_case=3;
    }

    switch(cmd_case)
    {
    //Check to see if in commanding states
    case (1): //MASTER CASE
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

        break;
} //END CASE 1 MASTER
    case (2): //MONITOR selected
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

    break;
} //END CASE 2 MONITOR
    case (3): //SIM selected
    {

        //COMMENTED OUT FOR WIP SIM CONNECTIONS
        QString current_time=QTime::currentTime().toString();
        //Print error in messages window
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append(current_time+": "+"COMMAND NOT SENT: LINK SIM");
        ui->txt_messages_out->setTextColor(Qt::black);

        //Print error in prompt window too
        ui->cmd_txt_messages->setTextColor(Qt::red);
        ui->cmd_txt_messages->append(current_time+": "+"<ERROR> LINK SIM: COMMAND NOT SENT"); //update prompt with red text
        ui->cmd_txt_messages->setTextColor(Qt::black);


        //WIP SIM CONNECTIONS


        break;
}//END CASE 3 SIM CONNECTION
    default:
    {
        QString current_time=QTime::currentTime().toString();
        //Print error in messages window
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append(current_time+": "+"COMMAND NOT SENT: NO LINK");
        ui->txt_messages_out->setTextColor(Qt::black);

        //Print error in prompt window too
        ui->cmd_txt_messages->setTextColor(Qt::red);
        ui->cmd_txt_messages->append(current_time+": "+"<ERROR> NO LINK"); //update prompt with red text
        ui->cmd_txt_messages->setTextColor(Qt::black);
} //END CASE DEFAULT

    } //END SWTICH CASE
 ui->txt_prompt->clear();



}


void MainWindow::on_btn_quit_clicked() //Closes main window
{
    close();
}

void MainWindow::start_listeners() //Spawns telemetry listeners
{

    /*
     * //Commented out for WIP SWITCH CASES
    //Set the process path
    QString commandtosend="./../../../../testing/sgs_sim_ieu_test/sgs/placeHoldTelem.sh"; //placeholder function, generates random telemetry packets to figure out updating GUI
    //Define listeners as a QProcess
    listeners=new QProcess(this);
    connect(listeners,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_telem())); //connect the data ready signal to the telem processor.
    listeners->start(commandtosend); //initiate the process asynchronously
    */

    //WIP SWTICH CASES
    /*
     * TODO
     * Fix case 3 so sim_ieu_tlm_loop.sh does not return an error when called
     *error reads "./../../../../testing/sgs_sim_ieu_test/sim_ieu/sim_ieu_tlm_loop.sh: line 6: ./sim_ieu_send_tlm_pkt: No such file or directory"
     * May need to change path in sim_ieu_tlm_loop.sh to be more global or non-environment specific.
     */


    int cmd_case;
    //Figure out what state is checked:
    if (ui->rad_btn_Master->isChecked())
    {
        cmd_case=1;
    }
    if (ui->rad_btn_Monitor->isChecked())
    {
        cmd_case=2;
    }
    if (ui->rad_btn_Sim->isChecked())
    {
        cmd_case=3;
    }

    switch (cmd_case)
    {
    case 1: //WIP MASTER
    {

        QString current_time=QTime::currentTime().toString();
        //Print error in messages window
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append(current_time+": "+"COMMAND NOT SENT: MASTER LINKS NOT ENABLED WIP");
        ui->txt_messages_out->setTextColor(Qt::black);

        //Print error in prompt window too
        ui->cmd_txt_messages->setTextColor(Qt::red);
        ui->cmd_txt_messages->append(current_time+": "+"<ERROR> MASTER LINKS NOT ENABLED WIP"); //update prompt with red text
        ui->cmd_txt_messages->setTextColor(Qt::black);
    }//END CASE MASTER

    case 2: //WIP MONITOR ONLY
    {
        QString current_time=QTime::currentTime().toString();
        //Print error in messages window
        ui->txt_messages_out->setTextColor(Qt::red);
        ui->txt_messages_out->append(current_time+": "+"COMMAND NOT SENT: MONITOR LINKS NOT ENABLED WIP");
        ui->txt_messages_out->setTextColor(Qt::black);

        //Print error in prompt window too
        ui->cmd_txt_messages->setTextColor(Qt::red);
        ui->cmd_txt_messages->append(current_time+": "+"<ERROR> MONITOR LINKS NOT ENABLED WIP"); //update prompt with red text
        ui->cmd_txt_messages->setTextColor(Qt::black);
    } //END MONITOR ONLY

    case 3://WIP SIM CONNECTION
    {
        //Configure ports for sim IEU to use
//Setup virtual serial ports.
        QString port_command="./../../../../testing/sgs_sim_ieu_test/sim_ieu/sim_ieu_port_setup.sh"; //Writes to STDERR for some reason
            //Define listeners as a QProcess
            sim_port_setup_listener=new QProcess(this);
            connect(sim_port_setup_listener,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_msgs())); //connect the data ready signal to the telem processor.
            connect(sim_port_setup_listener,SIGNAL(readyReadStandardError()),this,SLOT(print_to_msgs()));
            sim_port_setup_listener->start(port_command); //initiate the process asynchronously
//Start the gc_listener
       QString gc_listen_cmd="./../../../../testing/sgs_sim_ieu_test/sgs/gc_rcv_tlm";
       sim_reader=new QProcess(this);
       connect(sim_reader,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_telem()));
       connect(sim_reader,SIGNAL(readyReadStandardError()),this,SLOT(print_to_telem()));
       sim_reader->start(gc_listen_cmd);

//Start the telemetry loop
       QString sim_send_tlm_cmd="./../../../../testing/sgs_sim_ieu_test/sim_ieu/sim_ieu_tlm_loop.sh";
            sim_tlm_loop=new QProcess(this);
            connect(sim_tlm_loop,SIGNAL(readyReadStandardError()),this,SLOT(print_to_msgs())); //Connect listeners to STDOUT and ERR
            connect(sim_tlm_loop,SIGNAL(readyReadStandardOutput()),this,SLOT(print_to_msgs()));
            sim_tlm_loop->start(sim_send_tlm_cmd);



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






//
////Take the telemetry and seperate the values, comma delimited using QString Section
//IEU_temp=output.section(',',0,0);
//IEU_volt=output.section(',',1,1);
//IMG_rate=output.section(',',2,2);
//HAS_img=output.section(',',3,3);
//MAG_rate=output.section(',',4,4);
//MAG_packets=output.section(',',5,5);
//cc_exetxt=output.section(',',6,6);
//cc_acktxt=output.section(',',7,7);
//cc_errtxt=output.section(',',8,8);

////Update the GUI's appropriate feilds
//ui->lbl_IEU_temp->setText(IEU_temp);
//ui->lbl_IEU_volt->setText(IEU_volt);
//ui->lbl_IMG_rate->setText(IMG_rate);
//ui->lbl_mag_rate->setText(MAG_rate);

////Check to see if there is a mag packet or Imaging packet


////Update telemetry counters
////Get the current telemetry packet count from GUI, this is a really bad way to do this, need to make a pointer or something
//QString currentCount=ui->lbl_telem_rx->text();
//int currentCountint=currentCount.toInt()+1;
//ui->lbl_telem_rx->setText(QString::number(currentCountint));

////Update imaging counters
//if (HAS_img.toInt()==1)
//{
//    QString currentCountImg=ui->lbl_IMG_rx->text();
//int currentImg=currentCountImg.toInt()+1;
//ui->lbl_IMG_rx->setText(QString::number(currentImg));
//}

////Update MAG counter
//if (MAG_packets.toInt()!=0)
//{
//    QString currentCountMag=ui->lbl_mag_rx->text();
//int currentMag=currentCountMag.toInt()+MAG_packets.toInt();
//ui->lbl_mag_rx->setText(QString::number(currentMag));
//}

////Update Command Counts
//ui->lbl_cmd_ack_count->setText(cc_acktxt);
//ui->lbl_cmd_error_count->setText(cc_errtxt);
//ui->lbl_cmd_exec_count->setText(cc_exetxt);

////Get command count descrepency (cc sent- cc ack) and update gui
//QString cc_senttxt=ui->lbl_cmd_sent_count->text();
//int cc_sent=cc_senttxt.toInt();
//int cc_ack=cc_acktxt.toInt();
//int cc_disc=cc_sent-cc_ack;

//ui->lbl_disc->setText(QString::number(cc_disc));




}//END OF SLOT print_to_telem

void MainWindow::stop_listeners()
{
    //Placeholder function to kill telemetry listener
    sim_reader->terminate();
    sim_tlm_loop->terminate();
    sim_port_setup_listener->terminate();

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

void MainWindow::sim_port_open_error() //Outputs errors when opening ports for sim connection, notifies user unsucessful sim setup
{

}

void MainWindow::print_to_msgs()
{
   QString stdout=sim_port_setup_listener->readAllStandardOutput();
   ui->txt_messages_out->append(stdout);
   QString stderr=sim_port_setup_listener->readAllStandardError();
   ui->txt_messages_out->setTextColor(Qt::red);
   ui->txt_messages_out->append(stderr);
   ui->txt_messages_out->setTextColor(Qt::black);

   stdout=sim_tlm_loop->readAllStandardOutput();
   ui->txt_messages_out->append(stdout);
    stderr=sim_tlm_loop->readAllStandardError();
   ui->txt_messages_out->setTextColor(Qt::red);
   ui->txt_messages_out->append(stderr);
   ui->txt_messages_out->setTextColor(Qt::black);




}









