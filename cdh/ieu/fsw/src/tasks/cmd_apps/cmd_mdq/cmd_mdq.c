///////////////////////////////////////////////////////////////////////////////
//
// Command MDQ (Magnetometer DAQ)
//
// Task responsible for executing magnetometer DAQ commands. Magnetometer DAQ
// commands are received in command transfer frames via synchronous message
// passing from the command executor task. 
//
// Command transfer frames consist of
//     - APID
//     - Packet name
//     - Execution time
//     - ATC flag
//     - Arguments
//
// The command's packet name is compared against know command packet names.
// When the command's packet name is matched, the command is executed. 
// Regardless of successful command execution, the task replies to the command
// executor task to reports its status of command execution.
//
// The DAQ is initialized in this task by calling a function that that enables
// channels 0, 1, and 2 and sets the sampling rate to 240 Hz for each channel
// (among other things).
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
// Created: January 22, 2019
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
#include <alchemy/timer.h> // Timer management services
#include <alchemy/sem.h>   // Semaphore services

// Header files:
#include <sems.h>         // Semaphore variable declarations
#include <init_mdq.h>     // Magnetometer DAQ initialization function
                          // declaration
#include <send_mdq_cmd.h> // Send Magnetometer DAQ command function declaration
#include <hk_tlm_var.h>   // Housekeeping telemetry variable declarations

// Macro definitions:
#define CMD_XFR_FRM_SIZE 15 // Command transfer frame size in bytes
#define RPLY_MSG_SIZE     1 // Command execution status reply message to
                            // command executor task size in bytes

#define CMD_BGNMDQSCAN  0x00 // Command: Begin MDQ scan
#define CMD_HALTMDQSCAN 0x01 // Command: Halt MDQ scan
#define CMD_NOOP      0x3FFF // Command: Non-operational

// Semaphore definitions:
RT_SEM cmd_mdq_sem;  // For exec_cmd and cmd_mdq task synchronization
RT_SEM read_mdq_sem; // For cmd_mdq and read_mdq task synchronization to
                     // indicate when DAQ is readable (scanning)

// Global variable definitions:
uint8_t mdq_scan_state = 0; // Magnetometer DAQ scanning state

void cmd_mdq(void* arg) {
    // Print:
    rt_printf("%d (CMD_MDQ_TASK) Task started\n",time(NULL));

    // Declarations and initialization:
    int8_t   ret_val; // Function return value
    uint32_t flw_id;  // Flow identifier for the message transaction

    uint16_t cmd_apid;           // Command APID
    uint16_t cmd_pkt_name;       // Command packet name
    uint8_t  cmd_atc_flg;        // Command ATC flag
    uint32_t cmd_exec_time_sec;  // Command execution time (seconds)
    uint16_t cmd_exec_time_msec; // Command execution time (milliseconds)
    uint32_t cmd_arg;            // Command arguments

    char cmd_xfr_frm_buf[CMD_XFR_FRM_SIZE]; // Buffer for command transfer
                                            // frame
    uint8_t cmd_exec_stat;                  // Buffer for command execution
                                            // status reply message

    RT_TASK_MCB cmd_xfr_frm_mcb; // For command transfer frame message from
                                 // command executor task
    RT_TASK_MCB rply_mcb;        // For command execution status reply message
                                 // to command executor task

    cmd_xfr_frm_mcb.data = cmd_xfr_frm_buf;  // Set message block buffer
    cmd_xfr_frm_mcb.size = CMD_XFR_FRM_SIZE; // Set message block buffer
                                             // size

    rply_mcb.data = &cmd_exec_stat; // Set reply message buffer
    rply_mcb.size = RPLY_MSG_SIZE;  // Set reply message size

    // Initialize magnetometer DAQ:
    ret_val = init_mdq();

    // Task synchronize with exec_cmd task
    // (tell task that it is now ready to receive frames)
    rt_printf("%d (CMD_MDQ_TASK) Ready to receive command transfer"
        " frames\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&cmd_mdq_sem);

    // Infinite loop to receive a command transfer frame from command executor
    // task via synchronous message passing. Match command packet name to
    // known command packet names to execute command.
    while(1) {
        // Receive command transfer frames from command executor task via
        // synchronous message:
        flw_id = rt_task_receive(&cmd_xfr_frm_mcb,\
            TM_INFINITE); // Will wait infinite amount of time for message 

        // Check success:
        if (flw_id > 0) {
            // Print
            rt_printf("%d (CMD_MDQ_TASK) Received command transfer"
                " frame\n",time(NULL));
        } else {
            // Print
            rt_printf("%d (CMD_MDQ_TASK) Error receiving command transfer"
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
        rt_printf("%d (CMD_MDQ_TASK) Checking command packet"
            " name...\n",time(NULL));

        // Switch to match command packet name to know command packet
        // names; execute command if match:
        switch (cmd_pkt_name) {
            case CMD_BGNMDQSCAN :
                // Check if DAQ is currently scanning. If not scanning, then
                // execute command. If scanning, then do not execute command
                // as this would seriously confuse the device:
                if (mdq_scan_state == 0) {
                    // Printstate
                    rt_printf("%d (CMD_MDQ_TASK) Starting magnetometer DAQ"
                        " scanning\n",time(NULL));

                    // Send command to start scanning:
                    ret_val = send_mdq_cmd("start 0");

                    // Check success:
                    if (ret_val == 0) {
                        // Print:
                        rt_printf("%d (CMD_IMG_TASK) Command executed"
                            " successfully\n",time(NULL));

                        // Signal read mdq task to start reading data from DAQ:
                        rt_sem_v(&read_mdq_sem);

                        // Set DAQ scanning flag:
                        mdq_scan_state = 1;

                        // Set state message data field to indicate command
                        // executed:
                        cmd_exec_stat = 1;
                    } else if (ret_val < 0) {
                        // Print:
                        rt_printf("%d (CMD_IMG_TASK) Command did not execute"
                            "\n",time(NULL));

                        // Set reply message data field to indicate command
                        // did not execute:
                        cmd_exec_stat = 0;
                    }
                } else if (mdq_scan_state == 1) {
                    // Printstate
                    rt_printf("%d (CMD_MDQ_TASK) Magnetometer DAQ is already"
                        " scanning; command transfer frame ignored\n",\
                        time(NULL));

                    // Set reply message data field to indicate command
                    // did not execute:
                    cmd_exec_stat = 0;
                }
                // Exit switch:
                break;
            case CMD_HALTMDQSCAN :
                // Check if DAQ is currently scanning. If scanning, then
                // execute command. If not, then do not execute command.
                if (mdq_scan_state == 1) {
                    // Printstate
                    rt_printf("%d (CMD_MDQ_TASK) Halting magnetometer DAQ"
                        " scanning\n",time(NULL));

                    // Signal read mdq task to stop reading data from DAQ:
                    rt_sem_p(&read_mdq_sem,TM_INFINITE);

                    // Send command to start scanning:
                    ret_val = send_mdq_cmd("stop");

                    // Check success:
                    if (ret_val == 0) {
                        // Print:
                        rt_printf("%d (CMD_IMG_TASK) Command executed"
                            " successfully\n",time(NULL));

                        // Send command to start scanning:
                        ret_val = send_mdq_cmd("led 6");

                        // Set DAQ scanning state:
                        mdq_scan_state = 0; // Not scanning

                        // Set state message data field to indicate command
                        // executed:
                        cmd_exec_stat = 1;
                    } else if (ret_val < 0) {
                        // Print:
                        rt_printf("%d (CMD_IMG_TASK) Command did not execute"
                            "\n",time(NULL));

                        // Set reply message data field to indicate command
                        // did not execute:
                        cmd_exec_stat = 0;
                    }
                } else if (mdq_scan_state == 0) {
                    // Printstate
                    rt_printf("%d (CMD_MDQ_TASK) Magnetometer DAQ is not"
                        " scanning; command transfer frame ignored\n",\
                        time(NULL));

                    // Set reply message data field to indicate command
                    // did not execute:
                    cmd_exec_stat = 0;
                }
                // Exit switch:
                break;
            case CMD_NOOP :
                // Print:
                rt_printf("%d (CMD_MDQ_TASK) Executing NOOP command"
                    "\n",time(NULL));

                // Execute command and check for success:
                // HERE HERE HERE HERE
                ret_val = 1; // PLACEHOLDER

                // Check success:
                if (ret_val > 0) {
                    // Print:
                    rt_printf("%d (CMD_MDQ_TASK) Command executed"
                        " successfully\n",time(NULL));

                    // Set reply message data field to indicate command
                    // executed:
                    cmd_exec_stat = 1; 
                } else {
                    // Print:
                    rt_printf("%d (CMD_MDQ_TASK) Command did not execute"
                        "\n",time(NULL));

                    // Set reply message data field to indicate command
                    // executed:
                    cmd_exec_stat = 0; 
                }

                // Exit switch:
                break;
            // If command packet name does not match:
            default :
                // Print:
                rt_printf("%d (CMD_MDQ_TASK) Command packet is invalid;" 
                    " command transfer frame ignored\n",\
                    time(NULL));

                // Set reply message data field to indicate command
                // did not execute:
                cmd_exec_stat = 0;
        }

        // Reply to command executor task with command execution status:
        ret_val = rt_task_reply(flw_id,&rply_mcb);

        // Check for success:
        if (ret_val == 0) {
            // Print:
            rt_printf("%d (CMD_MDQ_TASK) Reply message sent to execute command"
                " task\n",time(NULL));
        } else {
            // Print:
            rt_printf("%d (CMD_MDQ_TASK) Error sending reply message to execute"
                " command task\n",time(NULL));
        }
    }
    
    // This will never be reached:
    return;
}