///////////////////////////////////////////////////////////////////////////////
//
// Command Software
//
// Task responsible for executing software commands. Software commands are
// received in command transfer frames via synchronous message passing from
// the command executor task. 
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
// Created: January 20, 2019
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
#include <sems.h> // Semaphore variable declarations

// Macro definitions:
#define CMD_XFR_FRM_SIZE 15 // Command transfer frame size in bytes
#define RPLY_MSG_SIZE     1 // Command execution status reply message to
                            // command executor task size in bytes

#define CMD_NOOP 0x00 // Command: Non-operationa
#define CMD_PBK  0x01 // Command: Playback

// Task definitions:
RT_TASK rtrv_file_task; // Retrieve file

// Semaphore definitions:
RT_SEM cmd_sw_sem;    // For exec_cmd and cmd_sw task synchronization
RT_SEM rtrv_file_sem; // For rtrv_file_task and cmd_sw_task
                      // synchronization 

void cmd_sw(void* arg) {
    // Print:
    rt_printf("%d (CMD_SW_TASK) Task started\n",time(NULL));

    // Task synchronize with retrieve file task:
    // (wait for task to be ready to receive command transfer frames)
    rt_printf("%d (CMD_SW_TASK) Waiting for retrieve file task to be ready"
        "\n",time(NULL));

    // Wait for signal:
    rt_sem_p(&rtrv_file_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (CMD_SW_TASK) Retrieve file task is ready;"
        " continuing\n",time(NULL));

    // Declarations and initialization:
    int8_t ret_val; // Function return value
    int8_t flw_id;  // Flow identifier returned by rt_task_receive

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

    // Task synchronize with execute command task
    // (tell task that it is now ready to receive frames)
    rt_printf("%d (CMD_SW_TASK) Ready to receive command transfer"
        " frames\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&cmd_sw_sem);

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
            rt_printf("%d (CMD_SW_TASK) Received command transfer"
                " frame\n",time(NULL));
        } else {
            // Print
            rt_printf("%d (CMD_SW_TASK) Error receiving command transfer"
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
        rt_printf("%d (CMD_SW_TASK) Checking command packet"
            " name...\n",time(NULL));

        // Switch to match command packet name to know command packet
        // names; execute command if match:
        switch (cmd_pkt_name) {
            case CMD_NOOP :
                // Print:
                rt_printf("%d (CMD_SW_TASK) Executing NOOP command"
                    "\n",time(NULL));

                // Execute command and check for success:
                // HERE HERE HERE HERE
                ret_val = 1; // PLACEHOLDER

                // Check success:
                if (ret_val > 0) {
                    // Print:
                    rt_printf("%d (CMD_SW_TASK) Command executed"
                        " successfully\n",time(NULL));

                    // Set reply message data field to indicate command
                    // executed:
                    cmd_exec_stat = 1; 
                } else {
                    // Print:
                    rt_printf("%d (CMD_SW_TASK) Command did not execute"
                        "\n",time(NULL));

                    // Set reply message data field to indicate command
                    // executed:
                    cmd_exec_stat = 0; 
                }

                // Exit switch:
                break;
            case CMD_PBK :
                // Print:
                rt_printf("%d (CMD_SW_TASK) Executing PBK command with"
                    " arguments: %u\n",time(NULL),cmd_arg);

                ret_val = rt_task_send(&rtrv_file_task,&cmd_xfr_frm_mcb,\
                    &rply_mcb,TM_INFINITE);

                if (ret_val > 0) {
                    // Print:
                    rt_printf("%d (CMD_SW_TASK) Reply message received"
                        " from retrieve file\n",time(NULL));
                } else { 
                    // Print:
                    rt_printf("%d (CMD_SW_TASK) Error sending command"
                        " transfer frame to retrieve file task\n",time(NULL));
                    // NEED ERROR HANDLING
                }
                // Exit switch:
                break;
            // If command packet name does not match:
            default :
                // Print:
                rt_printf("%d (CMD_SW_TASK) Command packet is invalid;" 
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
            rt_printf("%d (CMD_SW_TASK) Reply message sent to execute command"
                " task\n",time(NULL));
        } else {
            // Print:
            rt_printf("%d (CMD_SW_TASK) Error sending reply message to execute"
                " command task\n",time(NULL));
        }
    }
    
    // This will never be reached:
    return;
}