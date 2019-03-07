///////////////////////////////////////////////////////////////////////////////
//
// Get Housekeeping (HK) Telemetry
//
// Task responsible for retrievig values of counters (housekeeping telemetry),
// creating telemetry packet transfer frames, and send those frames to filter
// table task for either downlink or recording on storage for later downlink.
// This is done be reading variables defined in the housekeeping variable
// declaration header file.
//
// This is a periodic task with a frequency set during task creation. The
// frequency should nominally be 1 Hz (once every second).
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
// Created: January 28, 2019
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
#include <alchemy/task.h>  // Task management serives
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <msg_queues.h>          // Message queue variable declarations
#include <sems.h>                // Semaphore variable declarations
#include <hk_tlm_var.h>          // Housekeeping telemetry variable
                                 // declarations
#include <crt_tlm_pkt_xfr_frm.h> // Create telemetry packet transfer frame
                                 // function declaration
// Macro definitions:
#define HK_TLM_SIZE            28 // Housekeeping telemetry size in bytes
#define TLM_PKT_XFR_FRM_SIZE 1089 // Telemetry transfer frame size in bytes

#define APID_SW 0x00 // Software origin

// Message queue definitions:
RT_QUEUE flt_tbl_msg_queue; // For telemetry packet transfer frames
                            // (read_usb/get_hk_tlm --> flt_tbl_task)

// Semaphore definitions:
RT_SEM flt_tbl_sem;     // For flt_tbl_task, read_usb, and get_hk_tlm task
                        // synchronization

// Definitions for housekeeping telemetry variables:
uint8_t  val_telecmd_pkt_cnt;     // Valid telecommand packet counter
uint8_t  inv_telecmd_pkt_cnt;     // Invalid telecommand packet counter
uint8_t  rx_telecmd_pkt_cnt;      // Received telecommand packet count
uint8_t  val_cmd_cnt;             // Valid command counter
uint8_t  inv_cmd_cnt;             // Invalid command counter
uint8_t  cmd_exec_suc_cnt;        // Commands executed successfully counter
uint8_t  cmd_exec_err_cnt;        // Commands not executed (error) counter
uint16_t tlm_pkt_xfr_frm_seq_cnt; // Packet sequence count
uint16_t acq_img_cnt;             // Acquired images count
uint8_t  img_acq_prog_flag;       // Image acquisition in progress flag
uint8_t  mdq_scan_state;          // Magnetometer DAQ scanning state
uint8_t  ers_rly_swtch_state;     // Electrical relay switch state
uint8_t  flt_tbl_mode;            // Filter table mode
uint16_t img_accpt_cnt;           // Accepted images (from IPS) count
uint16_t img_rej_cnt;             // Rejected images (from IPS) count
uint32_t next_img_acq_tm;         // Next image acquisition time
uint32_t next_atc_tm;             // Next absolutely timed command time
uint8_t  pbk_prog_flg;            // Playback in progress flag

void get_hk_tlm(void* arg){
    // Print:
    rt_printf("%d (GET_HK_TLM_TASK) Task started\n",time(NULL));

    // Task synchronize with filter table task:
    // (wait for task to be ready to telemetry packet transfer frames)
    rt_printf("%d (GET_HK_TLM_TASK) Waiting for filter table task"
        " to be ready\n",time(NULL));

    // Wait for signals:
    rt_sem_p(&flt_tbl_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (GET_HK_TLM_TASK) Create telemetry packet task is" 
        " ready; continuing\n",time(NULL));

    // Definitions and initializations:
    int8_t ret_val; // Function return value

    uint8_t tlm_pkt_xfr_frm_grp_flg; // Packet sequence grouping flag

    char hk_tlm_buf[HK_TLM_SIZE]; // Buffer housekeeping telemetry

    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer

    // Print 
    rt_printf("%d (GET_HK_TLM_TASK) Ready to get housekeeping telemetry"
        " and create telemetry packets\n",time(NULL));

    // Infinite loop to get housekeeping telemetry and send to create
    // telemetry packet task via message queue:
    while (1) {
        // Increment sequence count:
        tlm_pkt_xfr_frm_seq_cnt++;

        // Force counter roll over at 16384:
        // (the field in the packet that sequence occupies is only 14 bits)
        if (tlm_pkt_xfr_frm_seq_cnt > 16383) {
            tlm_pkt_xfr_frm_seq_cnt = 1; // 1 because it's logical (0 ain't)
        }

        // Copy housekeeping telemetry to buffer:
        memcpy(hk_tlm_buf+0,&rx_telecmd_pkt_cnt,1);
        memcpy(hk_tlm_buf+1,&val_telecmd_pkt_cnt,1);
        memcpy(hk_tlm_buf+2,&inv_telecmd_pkt_cnt,1);
        memcpy(hk_tlm_buf+3,&val_cmd_cnt,1);
        memcpy(hk_tlm_buf+4,&inv_cmd_cnt,1);
        memcpy(hk_tlm_buf+5,&cmd_exec_suc_cnt,1);
        memcpy(hk_tlm_buf+6,&cmd_exec_err_cnt,1);
        memcpy(hk_tlm_buf+7,&tlm_pkt_xfr_frm_seq_cnt,2);
        memcpy(hk_tlm_buf+9,&acq_img_cnt,2);
        memcpy(hk_tlm_buf+11,&img_acq_prog_flag,1);
        memcpy(hk_tlm_buf+12,&ers_rly_swtch_state,1);
        memcpy(hk_tlm_buf+13,&mdq_scan_state,1);
        memcpy(hk_tlm_buf+14,&flt_tbl_mode,1);
        memcpy(hk_tlm_buf+15,&img_accpt_cnt,2);
        memcpy(hk_tlm_buf+17,&img_rej_cnt,2);
        memcpy(hk_tlm_buf+19,&next_img_acq_tm,4);
        memcpy(hk_tlm_buf+23,&next_atc_tm,4);
        memcpy(hk_tlm_buf+27,&pbk_prog_flg,1);

        // Set grouping flag:
        tlm_pkt_xfr_frm_grp_flg = 3; // Unsegmented data

        // Create transfer frame:
        crt_tlm_pkt_xfr_frm(hk_tlm_buf,HK_TLM_SIZE,\
            tlm_pkt_xfr_frm_buf,APID_SW,\
            tlm_pkt_xfr_frm_grp_flg,tlm_pkt_xfr_frm_seq_cnt);

        // Send transfer frame to filter table task via message queue:
        ret_val = rt_queue_write(&flt_tbl_msg_queue,&tlm_pkt_xfr_frm_buf,\
            TLM_PKT_XFR_FRM_SIZE,Q_NORMAL); // Append message to queue

        // Check success:
        if ((ret_val > 0) || (ret_val == 0)) {
            // Print:
            //rt_printf("%d (GET_HK_TLM_TASK) Telemetry packet transfer"
            //    " frame sent to filter table task\n",time(NULL));
        } else if (ret_val == -ENOMEM) {
            // Wait for a set time to allow filter table task to
            // process message queue:
            sleep(0.35);

            // Send transfer frame to filter table task via message queue:
            ret_val = rt_queue_write(&flt_tbl_msg_queue,&tlm_pkt_xfr_frm_buf,\
                TLM_PKT_XFR_FRM_SIZE,Q_NORMAL); // Append message to queue 
        } else {
            // Print:
            rt_printf("%d (GET_HK_TLM_TASK) Error sending telemetry"
                " packet transfer frame\n",time(NULL));
            // NEED ERROR HANDLING
        }

        // Release processor and wait for next period to execute again:
        rt_task_wait_period(NULL);
    }
}