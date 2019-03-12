///////////////////////////////////////////////////////////////////////////////
//
// Retrieve File
//
// Task responsible for playing back stored houskeeping telemetry, magnetometer
// DAQ or imaging data. This is done by retrieving files (saved telemetry
// packet transfer frames) and sending them to transmit telemetry packet task
// for downlink via message queue. Transfer frames consist of the following:
//     - Packet Identification
//       - APID (origin)
//       - Group flag (packet sequence)
//     - Creation Time
//       - Second
//       - Millisecond
//     - Telemetry Packet
//
// The command to playback housekeeping telemetry, magnetometer DAQ, or
// imaging data is received from command software task via synchronous message
// passing.
//
// File retrieval is done by reading directory listing files created by
// create file task. These listings are in time order such that files are
// downlinked oldest -> newest. Files are found as binary files in the
// following tree (names may not be correct but structure is)
//
//  |--raw_record_tlm
//     |-- mdq
//         |-- sec_msec.dat
//         |-- sec_msec.dat
//         |-- ...
//         |-- mdq_dir.ls
//     |-- img
//         |-- sec_msec
//             |-- 1.dat
//             |-- 2.dat
//             |-- ...
//             |-- n.dat
//         |-- sec_msec_dir.ls
//         |-- ...
//     |-- hk
//         |-- sec_msec.dat
//         |-- sec_msec.dat
//         |-- ...
//         |-- hk_dir.ls
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
// Created: February 3, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdlib.h>  // Standard library
#include <stdio.h>   // Standard input/output definitions
#include <unistd.h>  // UNIX standard function definitions
#include <errno.h>   // Error number definitions
#include <string.h>  // String function definitions 
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types
#include <dirent.h>  // Pseudo standard directory listing library

// Xenomai libraries:
#include <alchemy/task.h>  // Task management service
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations
#include <hk_tlm_var.h> // Housekeeping telemetry variable
                        // declarations

// Macro definitions:
#define CMD_XFR_FRM_SIZE       15 // Command transfer frame size in bytes
#define RPLY_MSG_SIZE           1 // Command execution status reply message to
                                  // command executor task size in bytes
#define TLM_PKT_XFR_FRM_SIZE 1089 // Telemetry packet transfer frame size in
                                  // bytes

#define ARG_HK  0x00 // Command argument: Housekeeping telemetry
#define ARG_MAG 0x01 // Command argument: Magnetometer
#define ARG_IMG 0x02 // Command argument: Imaging

// Message queue definitions:
RT_QUEUE tx_tlm_pkt_msg_queue; // For telemetry packets
                               // (flt_tbl_task/rtrv_file_task
                               // --> tx_tlm_pkt_task)

// Semaphore definitions:
RT_SEM tx_tlm_pkt_sem;   // For tx_tlm_pkt_task and flt_tbl_task
                         // synchronization
RT_SEM rtrv_file_sem;    // For rtrv_file_task and cmd_sw_task
                         // synchronization 

// Message control blocks definitions:
RT_TASK_MCB cmd_xfr_frm_rtrv_mcb; // For command transfer frame message from
                                  // command executor task
RT_TASK_MCB rply_rtrv_mcb;        // For command execution status reply message
                                  // to command executor task

// Global variables:
uint8_t pbk_prog_flg = 0; // Playback in progress flag

void rtrv_file(void* arg) {
	// Print:
    rt_printf("%d (RTRV_FILE_TASK) Task started\n",time(NULL));

    // Task synchronize with transmit telemetry packet task:
    // (wait for task to be ready to receive telemetry packets)
    rt_printf("%d (RTRV_FILE_TASK) Waiting for transmit telemetry packet"
        " task to be ready \n",time(NULL));

    // Wait for signal:
    rt_sem_p(&tx_tlm_pkt_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (RTRV_FILE_TASK) Transmit telemetry packet task is ready;"
        " continuing\n",time(NULL));

    // Definitions and initializations:
    uint16_t file_cnt = 0; // Retrieved file count
    int16_t ret_val;       // Function return value
    int8_t  flw_id;        // Flow identifier returned by rt_task_receive

    uint32_t cmd_arg; // Command APID

    DIR* dir;           // Directory
    struct dirent* ent; // Directory entry (file) 

    FILE* dir_ls_file_ptr;   // Directory listing file pointer
    FILE* src_dat_file_ptr;  // Source data file
    char file_line[100];     // File line
    char file_name[100];     // File name
    char file_path[100];     // File path

    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer

    char cmd_xfr_frm_buf[CMD_XFR_FRM_SIZE]; // Buffer for command transfer
                                            // frame

    uint8_t cmd_exec_stat; // Buffer for command execution status reply message

    cmd_xfr_frm_rtrv_mcb.data = cmd_xfr_frm_buf;  // Set message block buffer
    cmd_xfr_frm_rtrv_mcb.size = CMD_XFR_FRM_SIZE; // Set message block buffer
                                                  // size

    rply_rtrv_mcb.data = &cmd_exec_stat; // Set reply message buffer
    rply_rtrv_mcb.size = RPLY_MSG_SIZE;  // Set reply message size

    // Task synchronize with command software task
    // (tell task that it is now ready receive command transfer frames)
    rt_printf("%d (RTRV_FILE_TASK) Ready to receive command transfer"
        " frames\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&rtrv_file_sem);

    // Infinite loop to playback stored magnetometer DAQ or imaging source
    // data. The command to do so is received from command software task via
    // synchronous message passing and specifies which type of data is to be
    // played back.
    while (1) {
        // Receive command transfer frames from command software task via
        // synchronous messaging:
        flw_id = rt_task_receive(&cmd_xfr_frm_rtrv_mcb,\
            TM_INFINITE); // Will wait infinite amount of time for message 

        // Check success:
        if (flw_id > 0) {
            // Print
            rt_printf("%d (RTRV_FILE_TASK) Received command transfer"
                " frame\n",time(NULL));
        } else {
            // Print
            rt_printf("%d (RTRV_FILE_TASK) Error receiving command transfer"
                " frame\n",time(NULL));
            // NEED ERROR HANDLING
        }

        // Parse command transfer frame:
        memcpy(&cmd_arg,cmd_xfr_frm_buf+11,4);

        // Check command argument to retrieve files:
        if (cmd_arg == ARG_HK) { 
            // Print:
            rt_printf("%d (RTRV_FILE_TASK) Starting stored housekeeping"
                " data playback\n",time(NULL));

            // Set flag:
            pbk_prog_flg = 1; // In progress

            // Open directory listing file:
            dir_ls_file_ptr = fopen("../raw_record_tlm/hk/hk_dir.ls","r");

            // Loop to read directory listing file:
            while (fgets(file_line,sizeof(file_line),dir_ls_file_ptr) != NULL) {
                // Strip newline character:
                file_line[strcspn(file_line,"\n")] = 0;

                // Ignore directory listing file:
                if (strcmp(file_line,"hk_dir.ls") != 0) {
                    // Append directory to file name:
                    sprintf(file_path,"../raw_record_tlm/hk/%s",file_line);

                    // Open file to read
                    src_dat_file_ptr = fopen(file_path,"rb");

                    // Read file:
                    fread(&tlm_pkt_xfr_frm_buf,1,TLM_PKT_XFR_FRM_SIZE,\
                        src_dat_file_ptr);

                    // Send transfer frame to transmit telemetry packet task via
                    // message queue:
                    ret_val = rt_queue_write(&tx_tlm_pkt_msg_queue,\
                        &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                        Q_NORMAL); // Append message to queue

                    // Check success:
                    if ((ret_val > 0) || (ret_val == 0)) {
                        // Print:
                        rt_printf("%d (RTRV_FILE_TASK) Telemetry packet"
                            " transfer frame sent to transmit telemetry packet"
                            " task\n",time(NULL));
                    } else if (ret_val == -ENOMEM) {
                        // Wait for a set time to allow transmit task to
                        // process message queue:
                        sleep(0.35);

                        // Send transfer frame to filter table task via message
                        // queue:
                        ret_val = rt_queue_write(&tx_tlm_pkt_msg_queue,\
                            &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                            Q_NORMAL); // Append message to queue
                    } else {
                        // Print:
                        rt_printf("%d (RTRV_FILE_TASK) Error sending telemetry"
                            " packet transfer frame\n",time(NULL));
                        // NEED ERROR HANDLING
                    }

                    // Close file:
                    fclose(src_dat_file_ptr);

                    // Increment file count:
                    file_cnt++;
                }
            }

            // Print:
            rt_printf("%d (RTRV_FILE_TASK) Housekeeping data playback"
                " complete\n",time(NULL));

            // Close file:
            fclose(dir_ls_file_ptr);
        } else if (cmd_arg == ARG_MAG) {
            // Print:
            rt_printf("%d (RTRV_FILE_TASK) Starting stored magnetometer DAQ"
                " playback\n",time(NULL));

            // Set flag:
            pbk_prog_flg = 1; // In progress

            // Open directory listing file:
            dir_ls_file_ptr = fopen("../raw_record_tlm/mdq/mdq_dir.ls", "r");

            // Loop to read directory listing file:
            while (fgets(file_line,sizeof(file_line),dir_ls_file_ptr) != NULL) {
                // Strip newline character:
                file_line[strcspn(file_line,"\n")] = 0;

                // Ignore directory listing file:
                if (strcmp(file_line,"mdq_dir.ls") != 0) {
                    // Append directory to file name:
                    sprintf(file_path,"../raw_record_tlm/mdq/%s",file_line);

                    // Open file to read
                    src_dat_file_ptr = fopen(file_path,"rb");

                    // Read file:
                    fread(&tlm_pkt_xfr_frm_buf,1,TLM_PKT_XFR_FRM_SIZE,\
                        src_dat_file_ptr);

                    // Send transfer frame to transmit telemetry packet task
                    // via message queue:
                    ret_val = rt_queue_write(&tx_tlm_pkt_msg_queue,\
                        &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                        Q_NORMAL); // Append message to queue

                    // Check success:
                    if ((ret_val > 0) || (ret_val == 0)) {
                        // Print:
                        rt_printf("%d (RTRV_FILE_TASK) Telemetry packet"
                            " transfer frame sent to transmit telemetry packet"
                            " task\n",time(NULL));
                    } else if (ret_val == -ENOMEM) {
                        // Wait for a set time to allow transmit task to
                        // process message queue:
                        sleep(0.35);

                        // Send transfer frame to filter table task via message
                        // queue:
                        ret_val = rt_queue_write(&tx_tlm_pkt_msg_queue,\
                            &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                            Q_NORMAL); // Append message to queue
                    } else {
                        // Print:
                        rt_printf("%d (RTRV_FILE_TASK) Error sending telemetry"
                            " packet transfer frame\n",time(NULL));
                        // NEED ERROR HANDLING
                    }

                    // Close file:
                    fclose(src_dat_file_ptr);

                    // Increment file count:
                    file_cnt++;
                }
            }

            // Print:
            rt_printf("%d (RTRV_FILE_TASK) Magnetometer DAQ data playback"
                " complete\n",time(NULL));

            // Close file:
            fclose(dir_ls_file_ptr);
        } else if (cmd_arg == ARG_IMG) {
            // Print:
            rt_printf("%d (RTRV_FILE_TASK) Starting stored imaging data"
                " playback\n",time(NULL));

            // Set flag:
            pbk_prog_flg = 1; // In progress

            // Open directory:
            dir = opendir("../raw_record_tlm/img/");

            // Loop to read directory listing file:
            while ((ent = readdir(dir)) != NULL) {
                // Only accept listing files (and not directories):
                if(strstr(ent->d_name, "_dir.ls") != NULL) {
                    // Create full file name:
                    sprintf(file_name,"../raw_record_tlm/img/%s",ent->d_name);

                    // Open directory listing file:
                    dir_ls_file_ptr = fopen(file_name, "r");

                    // Loop to read directory listing file:
                    while (fgets(file_line,sizeof(file_line),\
                        dir_ls_file_ptr)) {
                        // Strip newline character:
                        file_line[strcspn(file_line,"\n")] = 0;

                        // Append directory to file name:
                        sprintf(file_path,"../raw_record_tlm/img/%.*s/%s",14,\
                            ent->d_name,file_line);

                        // Open file to read
                        src_dat_file_ptr = fopen(file_path,"rb");

                        // Read file:
                        ret_val = fread(&tlm_pkt_xfr_frm_buf,1,\
                            TLM_PKT_XFR_FRM_SIZE,src_dat_file_ptr);

                        // Send transfer frame to transmit telemetry packet
                        // task via message queue:
                        ret_val = rt_queue_write(&tx_tlm_pkt_msg_queue,\
                            &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                            Q_NORMAL); // Append message to queue

                        // Check success:
                        if ((ret_val > 0) || (ret_val == 0)) {
                            // Print:
                            rt_printf("%d (RTRV_FILE_TASK) Telemetry packet"
                                " transfer frame sent to transmit telemetry"
                                " packet task\n",time(NULL));
                        } else if (ret_val == -ENOMEM) {
                            // Wait for a set time to allow transmit task to
                            // process message queue:
                            sleep(0.35);

                            // Send transfer frame to filter table task via
                            // message queue:
                            ret_val = rt_queue_write(&tx_tlm_pkt_msg_queue,\
                                &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                                Q_NORMAL); // Append message to queue
                        } else {
                            // Print:
                            rt_printf("%d (RTRV_FILE_TASK) Error sending"
                                " telemetry packet transfer frame\n",\
                                time(NULL));
                            // NEED ERROR HANDLING
                        }
                        // Close file:
                        fclose(src_dat_file_ptr);

                        // Increment file count:
                        file_cnt++;
                    }
                    // Close listing file:
                    fclose(dir_ls_file_ptr);
                }
            }
            // Print:
            rt_printf("%d (RTRV_FILE_TASK) Image data playback"
                " complete\n",time(NULL));

            // Close directory:
            closedir(dir);
        } else {
          // Print:
            rt_printf("%d (RTRV_FILE_TASK) Command argument not recognized; "
                " ignoring command transfer frame\n",time(NULL));

            // Set file count:
            file_cnt = 0;
        }

        // Check execution status:
        if (file_cnt != 0) {
            // Set execution status to successful:
            cmd_exec_stat = 1;
        } else {
            // Set execution status to unsuccessful:
            cmd_exec_stat = 0;
        }

        // Set flag:
        pbk_prog_flg = 0; // Idle

        // Reply to command executor task with command execution status:
        ret_val = rt_task_reply(flw_id,&rply_rtrv_mcb);

        // Check for success:
        if (ret_val == 0) {
            // Print:
            rt_printf("%d (RTRV_FILE_TASK) Reply message sent to command"
                " software task\n",time(NULL));
        } else {
            // Print:
            rt_printf("%d (RTRV_FILE_TASK) Error sending reply message to"
                " execute command task\n",time(NULL));
        }

        // Reset file retrieval count:
        file_cnt = 0;
    }
}
