///////////////////////////////////////////////////////////////////////////////
//
// (Telemetry) Filter Table
//
// Task responsible for directing telemetry transfer frames for either downlink
// or storage for later downlink. Transfer frames are received via message
// queue from read_usb tasks, get_hk_tlm task, or retrieve file task. 
// Telemetry packet transfer frames are fixed length and consist of
//     - Packet Identification
//       - APID (origin)
//       - Group flag (packet sequence)
//     - Creation Time
//       - Second
//       - Millisecond
//     - Telemetry Packet
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
#include <hk_tlm_var.h> // Housekeeping telemetry variable declarations

// Macro definitions:
#define TLM_PKT_XFR_FRM_SIZE 1089 // Telemetry transfer frame size in bytes

#define APID_SW  0x00 // Software origin
#define APID_IMG 0x64 // Image origin
#define APID_MDQ 0xC8 // Magnetometer DAQ origin

// Message queue definitions:
RT_QUEUE flt_tbl_msg_queue;     // For telemetry packet transfer frames
                                // (read_mdq/img/get_hk_tlm --> flt_tbl_task)
RT_QUEUE tx_tlm_pkt_msg_queue;  // For telemetry packets
                                // (flt_tbl_task/rtrv_file_task
                                // --> tx_tlm_pkt_task)
RT_QUEUE crt_file_msg_queue;    // For telemetry packet transfer frames
                                // (flt_tbl_task --> crt_file_task)

// Semaphore definitions:
RT_SEM flt_tbl_sem;     // For flt_tbl_task, read_mdq/img, and get_hk_tlm task
                        // synchronization
RT_SEM tx_tlm_pkt_sem;  // For tx_tlm_pkt_task and flt_tbl_task
                        // synchronization
RT_SEM crt_file_sem;    // For crt_file_task and flt_tbl_task synchronization 

// Global variable definitions:
uint8_t flt_tbl_mode = 0; // Filter table mode

void flt_tbl(void* arg) {
    // Print:
    rt_printf("%d (FLT_TBL_TASK) Task started\n",time(NULL));

    // Task synchronize with transmit telemetry packet task:
    // (wait for task to be ready to receive telemetry packets)
    rt_printf("%d (FLT_TBL_TASK) Waiting for transmit telemetry packet"
        " task to be ready \n",time(NULL));

    // Wait for signal:
    rt_sem_p(&tx_tlm_pkt_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (FLT_TBL_TASK) Transmit telemetry packet task is ready;"
        " continuing\n",time(NULL));

    // Task synchronize with crt_file task:
    // (wait for task to be ready to receive telemetry packets)
    rt_printf("%d (FLT_TBL_TASK) Waiting for create file"
        " task to be ready \n",time(NULL));

    // Wait for signal:
    rt_sem_p(&crt_file_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (FLT_TBL_TASK) Create file task is ready continuing\n",\
        time(NULL));  

    // Definitions and initializations:
    uint8_t i;
    int16_t ret_val = 0; // Function return value

    uint8_t dl_tlm_pkt_flg;   // Downlink telemetry packet flag
    uint8_t rcrd_tlm_pkt_flg; // Record telemetry packet flag

    uint16_t tlm_pkt_xfr_frm_apid; // Telemetry packet transfer frame origin

    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer

    // Task synchronize with read_usb and get_hk_tlm tasks:
    // (tell task that it is now ready to receive telemetry packet transfer
    // frames)
    rt_printf("%d (FLT_TBL_TASK) Filter table task ready to receive telemetry"
        " packet transfer frames\n",time(NULL));
    
    // Signal:
    for (i = 0; i < 3; ++i)
        rt_sem_v(&flt_tbl_sem);

    // Infinite loop to receive telemetry packet transfer frames via message
    // queue and direct to either transmit telemetry packet task or create file
    // task:
    while (1) {
        // Read transfer frame from message queue:
        ret_val = rt_queue_read(&flt_tbl_msg_queue,\
            &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,TM_INFINITE);

        // Check success:
        if (ret_val == TLM_PKT_XFR_FRM_SIZE) {
            // Print
            rt_printf("%d (FLT_TBL_TASK) Received telemetry packet transfer"
                " frame\n",time(NULL));
        // Error:
        } else {
            // Print
            rt_printf("%d (FLT_TBL_TASK) Error receiving telemetry packet"
                " transfer frame\n",time(NULL));
            // NEED ERROR HANDLING                                          
        }

        // Get APID from transfer frame:
        memcpy(&tlm_pkt_xfr_frm_apid,tlm_pkt_xfr_frm_buf+0,2);

        // Direct transfer frame based on APID:
        if (tlm_pkt_xfr_frm_apid == APID_SW) {
            // Set direction flags:
            dl_tlm_pkt_flg = 1;   // Downlink packet
            rcrd_tlm_pkt_flg = 0; // Do not record
        } else if (tlm_pkt_xfr_frm_apid == APID_MDQ) {
            // Set direction flags:
            dl_tlm_pkt_flg = 1;   // Downlink packet
            rcrd_tlm_pkt_flg = 0; // Do not record
        } else if (tlm_pkt_xfr_frm_apid == APID_IMG) {
            // Set direction flags:
            dl_tlm_pkt_flg = 1;   // Downlink packet
            rcrd_tlm_pkt_flg = 0; // Do not record
        }

        // If directed to downlink, send transfer frame to transmit telemetry
        // packet via message queue:
        if (dl_tlm_pkt_flg == 1) {
            // Print:
            rt_printf("%d (FLT_TBL_TASK) Telemetry packet will be"
                " downlinked\n",time(NULL));

            // Send transfer frame to transmit telemetry packet task via
            // message queue:
            ret_val = rt_queue_write(&tx_tlm_pkt_msg_queue,\
                &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,Q_NORMAL);

            // Check success:
            if ((ret_val > 0) || (ret_val == 0)) {
                // Print:
                rt_printf("%d (FLT_TBL_TASK) Telemetry packet transfer"
                    " frame sent to transmit telemetry packet task\n",\
                    time(NULL));
            } else if (ret_val == -ENOMEM) {
                // Wait for a set time to allow transmit packet task to
                // process message queue:
                sleep(0.35);

                // Send transfer frame to filter table task via message queue:
                ret_val = rt_queue_write(&tx_tlm_pkt_msg_queue,\
                &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,Q_NORMAL);
            } else {
                // Print:
                rt_printf("%d (FLT_TBL_TASK) Error sending telemetry"
                    " packet transfer frame\n",time(NULL));
                // NEED ERROR HANDLING
            }
        } else {
            // Print:
            //rt_printf("%d (FLT_TBL_TASK) Telemetry packet will not be"
                //" downlinked\n",time(NULL));
        }

        // If directed to record, send transfer frame to create file task via
        // message queue:
        if (rcrd_tlm_pkt_flg == 1) {
            // Print:
            rt_printf("%d (FLT_TBL_TASK) Telemetry packet will be recorded\n",\
                time(NULL));

            // Send transfer frame to create file tasks to record them to
            // storage:
            ret_val = rt_queue_write(&crt_file_msg_queue,&tlm_pkt_xfr_frm_buf,\
                TLM_PKT_XFR_FRM_SIZE,Q_NORMAL); // Append message to queue

            // Check success:
            if ((ret_val > 0) || (ret_val == 0)) {
                // Print:
                rt_printf("%d (FLT_TBL_TASK) Telemetry packet transfer"
                    " frame sent to create file task\n",\
                    time(NULL));
            } else {
                // Print:
                rt_printf("%d (FLT_TBL_TASK) Error sending telemetry"
                    " packet transfer frame\n",time(NULL));
                // NEED ERROR HANDLING
            }
        } else {
            // Print:
            //rt_printf("%d (FLT_TBL_TASK) Telemetry packet will not be"
            //    " recorded\n",time(NULL));
        }

    }
}