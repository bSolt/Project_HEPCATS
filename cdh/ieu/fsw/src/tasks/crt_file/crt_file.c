///////////////////////////////////////////////////////////////////////////////
//
// Retrieve File
//
// Task responsible for creating files and storing them from received telemetry
// packet transfer frames via message queue from filter table task. Transfer
// frames consist of the following:
//     - Packet Identification
//       - APID (origin)
//       - Group flag (packet sequence)
//     - Creation Time
//       - Second
//       - Millisecond
//     - Telemetry Packet
//
// Files are created as binary files and stored in the following tree 
// (names may not be correct but structure is)
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
// Housekeeping telemetry and magnetometer DAQ source data is saved in one
// directories because file numbers are much lower than imaging. Images are
// kept in seperate directories as the number of files is excessive.
// 
// In each parent directory, directory listing files are created and updated
// for retrieve file task. The order of listings is oldest -> newest in order
// to playback files in time order.
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
#include <stdlib.h>  // Standard library.
#include <stdio.h>   // Standard input/output definitions
#include <unistd.h>  // UNIX standard function definitions
#include <errno.h>   // Error number definitions
#include <string.h>  // String function definitions 
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types

// Xenomai libraries:
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations

// Macro definitions:
#define TLM_PKT_XFR_FRM_SIZE 1089 // Telemetry transfer frame size in bytes

#define APID_SW 0x00  // Software origin
#define APID_IMG 0x64 // Image destination
#define APID_MDQ 0xC8 // Magnetometer DAQ destination

// Message queue definitions:
RT_QUEUE crt_file_msg_queue; // For telemetry packet transfer frames
                             // (flt_tbl_task --> crt_file_task)

// Semaphore definitions:
RT_SEM crt_file_sem; // For crt_file_task and flt_tbl_task synchronization 

void crt_file(void* arg) {
	// Print:
    rt_printf("%d (CRT_FILE_TASK) Task started\n",time(NULL));

    // Definitions and initialization:
    int16_t  ret_val; // Function return value
    uint16_t i;   // Counter

    uint16_t tlm_pkt_xfr_frm_apid;    // Telemetry packet transfer frame origin
    uint16_t tlm_pkt_xfr_frm_grp_flg; // Telemetry packet transfer frame packet
                                      // sequence
    uint32_t tlm_pkt_xfr_frm_sec;     // Telemetry packet transfer frame (sec)
    uint16_t tlm_pkt_xfr_frm_msec;    // Telemetry packet transfer frame time
                                      // (millisecond)

    FILE* file_ptr; // File pointer

    char file_name[200]; // File name
    char dir_name[200];  // directory name
    char sys_cmd[200];   // System command

    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer

    /// Task synchronize with filter table task
    // (tell task that it is now ready to receive transfer frames)
    rt_printf("%d (CRT_FILE_TASK) Ready to receive telemetry packet transfer"
        " frames\n",time(NULL));
    
    // Signal:
    rt_sem_v(&crt_file_sem);

    // Infinite loop to receive telemetry packet transfer frames via message
    // queue and record them to storage as individual files. For magnetometer,
    // only create file. For imaging, create new directory for start of new
    // image and then only create files in said directory.
    while (1) {
        // Read transfer frame from message queue:
        ret_val = rt_queue_read(&crt_file_msg_queue,\
            &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,TM_INFINITE);

        // Check success:
        if (ret_val == TLM_PKT_XFR_FRM_SIZE) {
            // Print
            rt_printf("%d (CRT_FILE_TASK) Received telemetry packet transfer"
                " frame\n",time(NULL));
        // Error:
        } else {
            // Print
            rt_printf("%d (CRT_FILE_TASK) Error receiving telemetry packet"
                " transfer frame\n",time(NULL));
            // NEED ERROR HANDLING                                          
        }

        // Get APID, time, and grouping flag of transfer frame:
        memcpy(&tlm_pkt_xfr_frm_apid,tlm_pkt_xfr_frm_buf+0,2);
        memcpy(&tlm_pkt_xfr_frm_grp_flg,tlm_pkt_xfr_frm_buf+2,1);
        memcpy(&tlm_pkt_xfr_frm_sec,tlm_pkt_xfr_frm_buf+3,4);
        memcpy(&tlm_pkt_xfr_frm_msec,tlm_pkt_xfr_frm_buf+7,2);

        // Check APID to handle how file is saved. Magnetometer DAQ transfer
        // frames are saved under one directory while imaging is saved in
        // different directories to group images.
        if (tlm_pkt_xfr_frm_apid == APID_SW) { 
            // Set file name dynamically:
            // (Name with format seconds_milliseconds.bin)
            sprintf(file_name,"../raw_record_tlm/hk/%u_%u.bin",\
                tlm_pkt_xfr_frm_sec,tlm_pkt_xfr_frm_msec);

            // Create listing directory command:
            sprintf(sys_cmd,"ls ../raw_record_tlm/hk/ >"
                " ../raw_record_tlm/hk/hk_dir.ls");
        } else if (tlm_pkt_xfr_frm_apid == APID_MDQ) {
            // Set file name dynamically:
            // (Name with format seconds_milliseconds.bin)
            sprintf(file_name,"../raw_record_tlm/mdq/%u_%u.bin",\
                tlm_pkt_xfr_frm_sec,tlm_pkt_xfr_frm_msec);

            // Create listing directory command:
            sprintf(sys_cmd,"ls ../raw_record_tlm/mdq/ >"
                " ../raw_record_tlm/mdq/mdq_dir.ls");
        } else if (tlm_pkt_xfr_frm_apid == APID_IMG) {
            // Check grouping flag. If first segment, create new directory;
            // otherwise, use current directory and just update file name:
            if (tlm_pkt_xfr_frm_grp_flg == 1) {
                // Create directory name:
                // (with format seconds_milliseconds/)
                sprintf(dir_name,"../raw_record_tlm/img/%u_%u",\
                    tlm_pkt_xfr_frm_sec,tlm_pkt_xfr_frm_msec);

                // Create make directory command:
                sprintf(sys_cmd,"mkdir %s/",dir_name);

                // Make directory:
                system(sys_cmd);

                // Set count:
                i = 1;

                // Create file name:
                // (with format #.bin)
                sprintf(file_name,"%s/%d.bin",dir_name,i);

                // Create empty command:
                sprintf(sys_cmd,"ls > /dev/null 2>&1");
            } else {
                // Increment count:
                i++;

                // Create file name:
                // (with format #.bin)
                sprintf(file_name,"%s/%d.bin",dir_name,i);

                // Check if last segment:
                if (tlm_pkt_xfr_frm_grp_flg == 2) {
                    // Create listing command:
                    sprintf(sys_cmd,"ls %s/ > %s_dir.ls",dir_name,dir_name);
                } else {
                    // Create empty command:
                    sprintf(sys_cmd,"ls > /dev/null 2>&1");
                }
            }
        }

        // Open a binary file:
        file_ptr = fopen(file_name,"wb");

        // Write transfer frame to file:
        fwrite(&tlm_pkt_xfr_frm_buf,1,TLM_PKT_XFR_FRM_SIZE,file_ptr);

        // Close file:
        fclose(file_ptr);

        // Update directory listing:
        system(sys_cmd);

        // Print
        rt_printf("%d (CRT_FILE_TASK) File created\n",time(NULL));
    }

    // Will never reach this
    return; 
}