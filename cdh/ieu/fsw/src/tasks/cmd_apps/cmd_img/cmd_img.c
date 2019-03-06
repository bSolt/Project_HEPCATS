///////////////////////////////////////////////////////////////////////////////
//
// Command Imaging
//
// Task responsible for executing imaging commands. Imaging commands are
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
#include <sems.h>       // Semaphore variable declarations
#include <init_cam.h>   // Initialize camera function declaration
#include <acq_img.h>    // Acquire image function declaration
#include <hk_tlm_var.h> // Housekeeping telemetry variable declarations

// Macro definitions:
#define CMD_XFR_FRM_SIZE 15 // Command transfer frame size in bytes
#define RPLY_MSG_SIZE     1 // Command execution status reply message to
                            // command executor task size in bytes

#define CMD_BGNIMGACQ   0x00  // Command: Begin image acquisition loop
#define CMD_HALTIMGACQ  0x01  // Command: Stop image acquisition loop
#define CMD_NOOP       0x3FFF // Command: Non-operational

// Semaphore definitions:
RT_SEM cmd_img_sem; // For exec_cmd and cmd_img task synchronization
RT_SEM new_img_sem; // For run_cam_sgl and read_usb_img task
                    // synchronization

// Global variable definitions:
uint8_t img_acq_prog_flag = 0; // Image acquisition in progress flag
uint16_t acq_img_cnt = 0;      // Acquired images count
uint8_t cmd_exec_suc_cnt;      // Commands executed successfully counter

void cmd_img(void* arg) {
    // Print:
    rt_printf("%d (CMD_IMG_TASK) Task started\n",time(NULL));

    // Declarations and initialization:
    int8_t   ret_val; // Function return value
    uint32_t flw_id;  // Flow identifier for the message transaction

    uint16_t cmd_apid;           // Command APID
    uint16_t cmd_pkt_name;       // Command packet name
    uint8_t  cmd_atc_flg;        // Command ATC flag
    uint32_t cmd_exec_time_sec;  // Command execution time (seconds)
    uint16_t cmd_exec_time_msec; // Command execution time (milliseconds)
    uint32_t cmd_arg;            // Command arguments

    uint32_t acq_dur;  // Image acquisition duration (seconds)
    uint32_t elp_time; // Elapsed time (seconds)

    uint16_t acq_ivl = 60;                  // Acquisition interval in seconds
    RTIME    acq_ivl_timeout = acq_ivl*1e9; // Acquisition interval timeo
                                            // in seconds

    char cmd_xfr_frm_buf[CMD_XFR_FRM_SIZE]; // Buffer for command transfer
                                            // frame
    int8_t cmd_exec_stat;                   // Buffer for command execution
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

    // Initialize camera:
    ret_val = init_cam();

    // Check success:
    // (If unsuccessful, then re-attempt until initialize is successful)
    while (ret_val < 0) {
        // Print:
        rt_printf("%d (CMD_IMG_TASK) Camera initialization unsuccessful;"
            " retrying in 5 seconds\n",time(NULL));

        // Wait:
        sleep(5); // 5 second retry

        // Initialize camera:
        ret_val = init_cam();
    }

    // Print:
    rt_printf("%d (CMD_IMG_TASK) Camera initialization complete\n",time(NULL));

    // Task synchronize with exec_cmd task
    // (tell task that it is now ready to receive frames)
    rt_printf("%d (CMD_IMG_TASK) Ready to receive command transfer"
        " frames\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&cmd_img_sem);

    // Infinite loop to receive a command transfer frame from command executor
    // task via synchronous message passing. Match command packet name to
    // known command packet names to execute command.
    while (1) {
        // Receive command transfer frames from command executor task via
        // synchronous message:
        flw_id = rt_task_receive(&cmd_xfr_frm_mcb,TM_INFINITE);

        // Address command label. This label is reached if image acquisition is
        // currently in progress and a command transfer frame is received:
        adr_cmd:

        // Check success:
        if (flw_id > 0) {
            // Print
            rt_printf("%d (CMD_IMG_TASK) Received command transfer"
                " frame\n",time(NULL));
        } else {
            // Print
            rt_printf("%d (CMD_IMG_TASK) Error receiving command transfer"
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
        rt_printf("%d (CMD_IMG_TASK) Checking command packet"
            " name...\n",time(NULL));

        // Switch to match command packet name to know command packet
        // names; execute command if match:
        switch (cmd_pkt_name) {
            case CMD_BGNIMGACQ :
                // Check to see if acquisition is currently in progress:
                if (img_acq_prog_flag == 0) {
                    // Print:
                    rt_printf("%d (CMD_IMG_TASK) Starting image acquisition"
                        " loop for duration %d seconds\n",time(NULL),cmd_arg);

                    // Set flag:
                    img_acq_prog_flag = 1; // Acquisition is in progress

                    // Save command argument to acquisition duration and set
                    // elapsed time:
                    acq_dur = cmd_arg;
                    elp_time = 0;

                    // Set command execution status as in progress:
                    cmd_exec_stat = -1;

                    // Release execute command task by reply to command
                    // executor task with command execution status as
                    // in progress:
                    ret_val = rt_task_reply(flw_id,&rply_mcb);

                    // Check for success:
                    if (ret_val == 0) {
                        // Print:
                        rt_printf("%d (CMD_IMG_TASK) Reply message sent to"
                            " execute command task\n",time(NULL));
                    } else {
                        // Print:
                        rt_printf("%d (CMD_IMG_TASK) Error sending reply"
                            " message to execute command task\n",time(NULL));
                    }

                    // Image acquisition label. This label is reached if a
                    // command interrupts image acquisition:
                    acq_img:

                    // Loop to acquire images for specified duration:
                    while (elp_time < acq_dur) {
                        // Call acquire image function:
                        ret_val = acq_img();

                        // Increment counter:
                        acq_img_cnt++;

                        // Increment elapsed time by acquisition interval:
                        elp_time += acq_ivl;

                        // Check if elapsed time is less then duration:
                        // (This is to check for the last loop)
                        if (elp_time < acq_dur) {
                            // Receive command transfer frames from command
                            // executor task via synchronous message if message
                            // is waiting. Otherwise, wait for acquisition
                            // timeout interval:
                            flw_id = rt_task_receive(&cmd_xfr_frm_mcb,\
                                acq_ivl_timeout);

                            // If message is received, interrupt acquisition loop
                            // to address command:
                            if (flw_id != -ETIMEDOUT) {
                                // Got to address command label:
                                goto adr_cmd;
                            }
                        }
                    }

                    // Set flag:
                    img_acq_prog_flag = 0; // Acquisition is not in progress

                    // Increment counter:
                    cmd_exec_suc_cnt++; // Command executed successfully

                    // Print:
                    rt_printf("%d (CMD_IMG_TASK) Image acquisition loop"
                        " complete; command executed successfully\n",\
                        time(NULL));

                    // Exit switch:
                    break;
                // Currently in progress
                } else {
                    // Print:
                    rt_printf("%d (CMD_IMG_TASK) Image acquisition loop"
                        " already in progress; command transfer frame"
                        " ignored\n",time(NULL));

                    // Set reply message data field to indicate command
                    // did not execute:
                    cmd_exec_stat = 0;

                    // Exit:
                    break; 
                }
            case CMD_HALTIMGACQ:
                // Check if acquisition loop is in progress:
                if (img_acq_prog_flag == 1) {
                    // Print:
                    rt_printf("%d (CMD_IMG_TASK) Halting image acquisition"
                        " loop\n",time(NULL));

                    // Set flag:
                    img_acq_prog_flag = 0; // Acquisition not in progress

                    // Set reply message data field to indicate command
                    // executed:
                    cmd_exec_stat = 1;
                } else {
                    // Print:
                    rt_printf("%d (CMD_IMG_TASK) Image acquisition loop not"
                        " in progress; command transfer frame ignored\n",\
                        time(NULL));

                    // Set reply message data field to indicate command
                    // did not execute:
                    cmd_exec_stat = 0;
                }

                // Exit switch:
                break;
            case CMD_NOOP :
                // Print:
                rt_printf("%d (CMD_IMG_TASK) Executing NOOP command"
                    "\n",time(NULL));

                // Execute command and check for success:
                // HERE HERE HERE HERE
                ret_val = 1; // PLACEHOLDER

                // Check success:
                if (ret_val > 0) {
                    // Print:
                    rt_printf("%d (CMD_IMG_TASK) Command executed"
                        " successfully\n",time(NULL));

                    // Set reply message data field to indicate command
                    // executed:
                    cmd_exec_stat = 1; 
                } else {
                    // Print:
                    rt_printf("%d (CMD_IMG_TASK) Command did not execute"
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
                rt_printf("%d (CMD_IMG_TASK) Command packet name is invalid;" 
                    " command transfer frame ignored\n",\
                    time(NULL));

                // Set reply message data field to indicate command
                // did not execute:
                cmd_exec_stat = 0;

                // Exit switch:
                break;
        }

        // Reply to command executor task with command execution status:
        ret_val = rt_task_reply(flw_id,&rply_mcb);

        // Check for success:
        if (ret_val == 0) {
            // Print:
            rt_printf("%d (CMD_IMG_TASK) Reply message sent to execute command"
                " task\n",time(NULL));
        } else {
            // Print:
            rt_printf("%d (CMD_IMG_TASK) Error sending reply message to execute"
                " command task\n",time(NULL));
        }

        // Check if image acquisition is in progress:
        if (img_acq_prog_flag == 1) {
            // Resume acquisition:
            goto acq_img;
        }
    }
    
    // This will never be reached:
    return;
}
