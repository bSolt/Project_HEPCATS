///////////////////////////////////////////////////////////////////////////////
//
// Command Scheduler
//
// Task responsible for executing absolutely timed commands. Absolutely timed
// commands are received from command executor task in command transfer frames
// via synchronous messaging.
//
// Command transfer frames consist of
//     - APID
//     - Packet name
//     - Execution time
//     - ATC flag
//     - Arguments
//
// DESCRIBE ALGORITHM HERE
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

// Semaphore definitions:
RT_SEM cmd_sched_sem; // For exec_cmd and cmd_sched task
                      // synchronization

// Message control blocks definitions:
RT_TASK_MCB cmd_xfr_frm_sched_mcb; // For command transfer frame from command
                                   // executor task
RT_TASK_MCB rply_sched_mcb;        // For command execution status reply message
                                   // to command executor task

void sched_cmd(void* arg) {
    // Print:
    rt_printf("%d (SCHED_CMD_TASK) Task started\n",time(NULL));

    // Declarations and initialization:
    int8_t ret_val; // Function return value
    int8_t flw_id;  // Flow identifier returned by rt_task_receive

    uint8_t sched_stat; // Command schedule status (successfully added
                        // to schedule flag)

    uint16_t cmd_apid;           // Command APID
    uint16_t cmd_pkt_name;       // Command packet name
    uint8_t  cmd_atc_flg;        // Command ATC flag
    uint32_t cmd_exec_time_sec;  // Command execution time (seconds)
    uint16_t cmd_exec_time_msec; // Command execution time (milliseconds)
    uint32_t cmd_arg;            // Command arguments

    char cmd_xfr_frm_buf[CMD_XFR_FRM_SIZE]; // Buffer for command transfer
                                            // frame

    cmd_xfr_frm_sched_mcb.data = cmd_xfr_frm_buf;  // Set message block buffer
    cmd_xfr_frm_sched_mcb.size = CMD_XFR_FRM_SIZE; // Set message block buffer
                                                   // size

    rply_sched_mcb.data = &sched_stat;
    rply_sched_mcb.size = RPLY_MSG_SIZE;

    // Task synchronize with exec_cmd task
    // (tell task that it is now ready to receive frames)
    rt_printf("%d (SCHED_CMD_TASK) Ready to receive command transfer"
        " frames\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&cmd_sched_sem);

    // Infinite loop to receive a command transfer frame from command executor
    // task via synchronous message passing.
    while(1) {
        // Receive command transfer frames from command executor task via
        // synchronous message:
        flw_id = rt_task_receive(&cmd_xfr_frm_sched_mcb,\
            TM_INFINITE); // Will wait infinite amount of time for message 

        // Check success:
        if (ret_val > 0) {
            // Print
            rt_printf("%d (SCHED_CMD_TASK) Received command transfer"
                " frame\n",time(NULL));
        } else {
            // Print
            rt_printf("%d (SCHED_CMD_TASK) Error receiving command transfer"
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

        sched_stat = 1;


        // Reply to command executor task with command execution status:
        ret_val = rt_task_reply(flw_id,&rply_sched_mcb);

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

    // Will never reach this:
    return;
}