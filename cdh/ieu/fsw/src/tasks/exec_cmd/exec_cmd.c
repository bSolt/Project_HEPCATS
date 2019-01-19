///////////////////////////////////////////////////////////////////////////////
//
// Execute Command
//
// Task responsible for executing commands received from either the
// telecommand processor task OR the command scheduler. Commands are received
// from each source in command transfer frames via the same message queue.
//
// Command transfer frames consist of
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
// Commands to execute now are sent to a command application task that
// associates with the command's APID via [FILL IN HERE]...
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

// Macro definitions:
#define CMD_XFR_FRM_SIZE 15 // Command transfer frame size in bytes
#define ATC_FLG_T         1 // ATC flag value indicating that the command is
                            // absolutely timed (true)
#define ATC_FLG_F         0 // ATC flag value indicating that the command is
                            // to execute now (false)
#define DEST_SW_APID   0x01 // Software APID

// Message queue definitions:
RT_QUEUE cmd_xfr_frm_msg_queue; // For command transfer frames
                                // (proc_telecmd_pkt_task/cmd_sched_task
                                //  --> exec_cmd_task)

// Semaphore definitions:
RT_SEM cmd_xfr_frm_sem; // For proc_telecmd_pkt_task and exec_cmd task
						// synchronization

void exec_cmd(void* arg) {
    // Print:
    rt_printf("%d (EXEC_CMD_TASK) Task started\n",time(NULL));

    // Definitions and initializations:
    int8_t ret_val; // Function return value

    uint16_t cmd_apid;           // Command APID
    uint16_t cmd_pkt_name;       // Command packet name
    uint8_t  cmd_atc_flg;        // Command ATC flag
    uint32_t cmd_exec_time_sec;  // Command execution time (seconds)
    uint16_t cmd_exec_time_msec; // Command execution time (milliseconds)
    uint32_t cmd_arg;		     // Command arguments

    char cmd_xfr_frm_buf[CMD_XFR_FRM_SIZE]; // Buffer for command transfer
                                            // frame

    // Task synchronize with proc_telecmd_pkt task
    // (tell task that it is now ready to receive frames)
    rt_printf("%d (EXEC_CMD_TASK) Ready to receive command transfer"
        " frames\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&cmd_xfr_frm_sem);

    // Infinte loop receive command transfer frames from telecommand processor
    // task or the command scheduler. When a command transfer frame is
    // received from the message queue, the ATC flag is checked to determine
    // if command is to be executed now or is absolutely timed (execute later).
    // If ATC flag = 0, then the command transfer frame is sent to the command
    // scheduler.
    while(1) {
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
            " time\n",time(NULL));

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
                case DEST_SW_APID :
                    // Print:
                    rt_printf("%d (EXEC_CMD_TASK) Command transfer frame"
                        " directed to APID=0x%X\n",time(NULL),DEST_SW_APID);
            } 
        } else if (cmd_atc_flg == ATC_FLG_T) {
            // Print:
            rt_printf("%d (EXEC_CMD_TASK) Command is absolutely"
                " timed; sending command to command scheduler\n",time(NULL));
        } else {
            // Print:
            rt_printf("%d (EXEC_CMD_TASK) Invalid absolutely timed command" 
                " flag; ignoring command\n",time(NULL));
        }

    }
    // Will never reach this:
    return;
}

/*
// Print results:
rt_printf("Command Identification\n");
rt_printf("  APID                 : %u\n",cmd_apid);
rt_printf("  Packet Name          : %u\n",cmd_pkt_name);
rt_printf("Command Execution Time\n");
rt_printf("      ATC Flag         : %u\n",cmd_atc_flg);
rt_printf("      Seconds          : %u\n",cmd_exec_time_sec);
rt_printf("      Milliseconds     : %u\n",cmd_exec_time_msec);
rt_printf("Command Argument\n");
rt_printf("      Command Argument : %u\n",cmd_arg);
rt_printf("\n");
*/
