///////////////////////////////////////////////////////////////////////////////
//
// Execute Command
//
// Task responsible for executing commands received from either the
// telecommand processor task OR the command scheduler. Commands are received
// from each source in command transfer frames via the same message queue.
//
// Command transfer frames are fixed length and consist of
//     - APID
//     - Packet name
//     - Execution time
//     - ATC flag
//     - Arguments
//
// When a command transfer frame is received from the message queue, the ATC
// flag is checked to determine if command is to be executed now or is
// absolutely timed. If ATC flag = 1, then the command is executed
// now (following a sanity check on the command execution time). If ATC
// flag = 0, them the command transfer frame is sent to the command scheduler.
//
// Commands to execute now are directed to command application tasks. The 
// command's APID is compared against known destination APIDs. When the
// command's APID is associated with the destination, the command transfer
// frame is sent to the respective command application task via synchronous
// messaging.
//
// Before addressing another command frame, a response from the command
// application is required indicating whether the command was
// successful or not.
//
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - Xenomai 3 / Cobalt
//
// Input Arguments:
// - N/A
//
// Output Arguments:
// - N/A
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: January 15, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdlib.h>  // Standard library
#include <unistd.h>  // UNIX standard function definitions
#include <errno.h>   // Error number definitions
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types

// Xenomai libraries:
#include <alchemy/task.h>  // Task management service
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <tasks.h>      // Task variable and function declarations
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations
#include <hk_tlm_var.h> // Housekeeping telemetry variable declarations

// Macro definitions:
#define CMD_XFR_FRM_SIZE 15 // Command transfer frame size in bytes
#define RPLY_MSG_SIZE     1 // Command execution status reply message to
                            // command executor task size in bytes

#define ATC_FLG_T  1 // ATC flag value indicating that the command
                     // is absolutely timed (true)
#define ATC_FLG_F  0 // ATC flag value indicating that the command
                     // is to execute now (false)

#define DEST_APID_SW  0x00 // Software destination APID
#define DEST_APID_IMG 0x64 // Image destination APID
#define DEST_APID_MDQ 0xC8 // Magnetometer DAQ destination APID
#define DEST_APID_ERS 0x12C // Electrical Relay Switch destination APID

// Task definitions:
RT_TASK dest_task; // Destination task of command (only local)

// Message queue definitions:
RT_QUEUE cmd_xfr_frm_msg_queue; // For command transfer frames
                                // (proc_telecmd_pkt_task/cmd_sched_task
                                //  --> exec_cmd_task)

// Semaphore definitions:
RT_SEM proc_telecmd_pkt_sem; // For proc_telecmd_pkt_task and exec_cmd task
                             // synchronization
RT_SEM cmd_sched_sem;        // For exec_cmd and cmd_sched task
                             // synchronization
RT_SEM cmd_sw_sem;           // For exec_cmd and cmd_sw task synchronization
RT_SEM cmd_img_sem;          // For exec_cmd and cmd_img task synchronization 
RT_SEM cmd_mdq_sem;          // For exec_cmd and cmd_mdq task synchronization
RT_SEM cmd_ers_sem;          // For exec_cmd and cmd_ers task synchronization

// Message control blocks definitions:
RT_TASK_MCB cmd_xfr_frm_mcb; // For command transfer frame to command
                             // application tasks
RT_TASK_MCB rply_mcb;        // For command execution status reply message
                             // command applications

// Housekeeping telemetry variable definitions:
uint8_t val_cmd_apid_cnt = 0; // Valid command counter
uint8_t inv_cmd_apid_cnt = 0; // Invalid command counter
uint8_t cmd_exec_suc_cnt = 0; // Commands executed successfully counter
uint8_t cmd_exec_err_cnt = 0; // Commands not executed (error) counter

void exec_cmd(void* arg) {
    // Print:
    rt_printf("%d (EXEC_CMD_TASK) Task started\n",time(NULL));

    // Task synchronize with command scheduler:
    // (wait for task to be ready to receive and processs frames)
    rt_printf("%d (EXEC_CMD_TASK) Waiting for command scheduler task"
        " to be ready\n",time(NULL));

    // Wait for signals:
    rt_sem_p(&cmd_sched_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (EXEC_CMD_TASK) Command scheduler task is" 
        " ready; continuing\n",time(NULL));

    // Task synchronize with command application tasks:
    // (wait for tasks to be ready to receive and process frames)
    rt_printf("%d (EXEC_CMD_TASK) Waiting for command application tasks"
        " to be ready\n",time(NULL));

    // Wait for signals:
    rt_sem_p(&cmd_sw_sem,TM_INFINITE);  // Command software
    rt_sem_p(&cmd_img_sem,TM_INFINITE); // Command imaging
    rt_sem_p(&cmd_mdq_sem,TM_INFINITE); // Command magnetometer DAQ
    rt_sem_p(&cmd_ers_sem,TM_INFINITE); // Command electrical relay switch

    // Print:
    rt_printf("%d (EXEC_CMD_TASK) Command application tasks are" 
        " ready; continuing\n",time(NULL));

    // Definitions and initializations:
    int8_t ret_val; // Function return value

    uint8_t cmd_exec_stat;    // Command execution status flag
    uint8_t val_apid_flg = 1; // Valid command APID flag

    char cmd_xfr_frm_buf[CMD_XFR_FRM_SIZE]; // Buffer for command transfer
                                            // frame

    uint16_t cmd_apid;           // Command APID
    uint16_t cmd_pkt_name;       // Command packet name
    uint8_t  cmd_atc_flg;        // Command ATC flag
    uint32_t cmd_exec_time_sec;  // Command execution time (seconds)
    uint16_t cmd_exec_time_msec; // Command execution time (milliseconds)
    uint32_t cmd_arg;            // Command arguments

    cmd_xfr_frm_mcb.data = cmd_xfr_frm_buf;  // Command transfer frame message
                                             // buffer
    cmd_xfr_frm_mcb.size = CMD_XFR_FRM_SIZE; // Command transfer frame message
                                             // size in bytes

    rply_mcb.data = &cmd_exec_stat; // Reply message buffer
    rply_mcb.size = RPLY_MSG_SIZE;  // Reply message size in bytes

    // Task synchronize with proc_telecmd_pkt task
    // (tell task that it is now ready to receive frames)
    rt_printf("%d (EXEC_CMD_TASK) Ready to receive command transfer"
        " frames\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&proc_telecmd_pkt_sem);

    // Infinte loop receive command transfer frames from telecommand processor
    // task or the command scheduler. When a command transfer frame is
    // received from the message queue, the ATC flag is checked to determine
    // if command is to be executed now or is absolutely timed (execute later).
    // If ATC flag = 0, then the command transfer frame is sent to the command
    // scheduler.
    while (1) {
        // Read command transfer frames from message queue:
        ret_val = rt_queue_read(&cmd_xfr_frm_msg_queue,&cmd_xfr_frm_buf,\
            CMD_XFR_FRM_SIZE,TM_INFINITE); // Will wait infinite amount of
                                           // time for message

        // Check success:
        if (ret_val == CMD_XFR_FRM_SIZE) {
            // Print
            rt_printf("%d (EXEC_CMD_TASK) Received command transfer"
                " frame\n",time(NULL));
        } else {
            // Print
            rt_printf("%d (EXEC_CMD_TASK) Error receiving command transfer"
                " frame\n",time(NULL));
            // NEED ERROR HANDLING
        }

        // Parse command transfer frame:
        memcpy(&cmd_apid,cmd_xfr_frm_buf+0,2);
        memcpy(&cmd_pkt_name,cmd_xfr_frm_buf+2,2);
        memcpy(&cmd_atc_flg,cmd_xfr_frm_buf+4,1);
        memcpy(&cmd_exec_time_sec,cmd_xfr_frm_buf+5,4);
        memcpy(&cmd_exec_time_msec,cmd_xfr_frm_buf+9,2);
        memcpy(&cmd_arg,cmd_xfr_frm_buf+11,4);

        // Print:
        rt_printf("%d (EXEC_CMD_TASK) Checking command execution"
            " time...\n",time(NULL));

        // Check for ATC flag. If ATC flag = 0, then execute the command now.
        // If ATC flag = 1, then send command transfer frame to the command
        // scheduler task. 
        if (cmd_atc_flg == ATC_FLG_F) {
            // Print:
            rt_printf("%d (EXEC_CMD_TASK) Command will execute"
                " NOW\n",time(NULL));

            // Switch to direct command to final destination by matching
            // the command's APID to a destination's APID:
            switch(cmd_apid) {
                // Software
                case DEST_APID_SW :
                    // Print:
                    rt_printf("%d (EXEC_CMD_TASK) Command APID is valid;"
                        " command transfer frame directed to command software"
                        " task\n",time(NULL));

                    // Set destination task:
                    dest_task = cmd_sw_task; // Command software task

                    // Exit switch:
                    break;
                // Imaging:
                case DEST_APID_IMG :
                    rt_printf("%d (EXEC_CMD_TASK) Command APID is valid;"
                        " command transfer frame directed to command imaging"
                        " task\n",time(NULL));

                    // Set destination task:
                    dest_task = cmd_img_task; // Command imaging task

                    // Exit switch:
                    break;
                // Magnetometer DAQ:
                case DEST_APID_MDQ :
                    rt_printf("%d (EXEC_CMD_TASK) Command APID is valid;"
                        " command transfer frame directed to command"
                        " magnetometer task\n",time(NULL));

                    // Set destination task:
                    dest_task = cmd_mdq_task; // Command magnetometer DAQ task

                    // Exit switch:
                    break;
                // Electrical power relay:
                case DEST_APID_ERS :
                    rt_printf("%d (EXEC_CMD_TASK) Command APID is valid;"
                        " command transfer frame directed to command"
                        " electrical relay task\n",time(NULL));

                    // Set destination task:
                    dest_task = cmd_ers_task; // Command electrical power relay task

                    // Exit switch:
                    break;
                // If command APID does not match:
                default :
                    // Print:
                    rt_printf("%d (EXEC_CMD_TASK) Command APID is invalid;" 
                        " command transfer frame ignored\n",\
                        time(NULL));

                    // Set valid APID flag to false:
                    val_apid_flg = 0;
            }
            
            // Check for valid APID (command APID matches known destinations):
            if (val_apid_flg == 1) {
                // Increase counter:
                ++val_cmd_apid_cnt;

                // Send command transfer frame to final destination via
                // synchronous message passing. Final destination is command
                // application task. Reply required from destination task for
                // command execution status:
                ret_val = rt_task_send(&dest_task,&cmd_xfr_frm_mcb,\
                    &rply_mcb,TM_INFINITE);

                if (ret_val > 0) {
                    // Print:
                    rt_printf("%d (EXEC_CMD_TASK) Reply message received"
                        " from command application task\n",time(NULL));
                } else { 
                    // Print:
                    rt_printf("%d (EXEC_CMD_TASK) Error sending command"
                        " transfer frame to command application\n",time(NULL));
                    // NEED ERROR HANDLING
                }

                // Check command execution status:
                if (cmd_exec_stat == 1) {
                    // Print:
                    rt_printf("%d (EXEC_CMD_TASK) Command executed" 
                        " successfully\n",time(NULL));

                    // Increment counter:
                    ++cmd_exec_suc_cnt;
                } else if (cmd_exec_stat == 0) {
                    // Print:
                    rt_printf("%d (EXEC_CMD_TASK) Command did not"
                        " execute\n",time(NULL));

                    // Increment counter:
                    ++cmd_exec_err_cnt;
                } else {
                    // Print:
                    rt_printf("%d (EXEC_CMD_TASK) Command execution status"
                    " is unkown\n",time(NULL));
                }
            // Invalid command APID:
            } else {
                // Increase counter:
                ++inv_cmd_apid_cnt;

                // Set valid APID flag to true:
                val_apid_flg = 1;
            }
        // Absolutely timed command:
        } else if (cmd_atc_flg == ATC_FLG_T) {
            // Print:
            rt_printf("%d (EXEC_CMD_TASK) Command is absolutely"
                " timed; sending command to command scheduler\n",time(NULL));
        // Unrecognized ATC flag:
        } else {
            // Print:
            rt_printf("%d (EXEC_CMD_TASK) Invalid absolutely timed command" 
                " flag; ignoring command transfer frame\n",time(NULL));
        }

    }
    // Will never reach this:
    return;
}

/*
// Print results:
rt_printf("Command Identification\n");
rt_printf("  APID             : %u\n",cmd_apid);
rt_printf("  Packet Name      : %u\n",cmd_pkt_name);
rt_printf("Command Execution Time\n");
rt_printf("  ATC Flag         : %u\n",cmd_atc_flg);
rt_printf("  Seconds          : %u\n",cmd_exec_time_sec);
rt_printf("  Milliseconds     : %u\n",cmd_exec_time_msec);
rt_printf("Command Argument\n");
rt_printf("  Command Argument : %u\n",cmd_arg);
rt_printf("\n");
*/
