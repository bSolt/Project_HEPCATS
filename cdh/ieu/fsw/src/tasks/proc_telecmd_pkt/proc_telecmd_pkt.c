///////////////////////////////////////////////////////////////////////////////
//
// Process Telecommand Packet
//
// Task responsible for processing telecommand packets received via message 
// queue from receive telecommand packet task. Packets are processed to 
// recover packet
//     - APID
//     - Packet name
//     - Execution time
//     - ATC flag
//     - Arguments
// These values are then packaged into a command transfer frame and sent 
// to the command executor task via message queue.
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
// Created: January 10, 2019
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
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations

// Macro definitions:
#define TELECMD_PKT_SIZE 20 // Telecommand packet size in bytes
#define CMD_XFR_FRM_SIZE 17 // Command transfer frame size in bytes

// Message queue definitions:
RT_QUEUE telecmd_pkt_msg_queue; // For telecommand packets
                                // (rx_telecmd_pkt_task 
                                //  --> proc_telecmd_pkt_task)
RT_QUEUE cmd_xfr_frm_msg_queue; // For command transfer frames
                                // (proc_telecmd_pkt_task --> cmd_exec_task)

// Semaphore definitions:
RT_SEM telecmd_pkt_sem; // For rx_telecmd_pkt_task and proc_telecmd_pkt_task
                        // task synchronization

void proc_telecmd_pkt(void) {
    // Print:
    rt_printf("%d (PROC_TELECMD_PKT_TASK) Task started\n",time(NULL));

    // Definitions and initializations:
    int8_t ret_val; // Function return value

    char  telecmd_pkt_buf[TELECMD_PKT_SIZE]; // Buffer for telecommand packet
    char* cmd_xfr_frm_buf[CMD_XFR_FRM_SIZE]; // Buffer for command transfer 
                                             // frame

    // Packet fields (raw data)
    // Packet header
    uint16_t pkt_hdr_pkt_id;      // Packet identification
    uint16_t pkt_hdr_pkt_seq_cnt; // Packet sequence count
    uint16_t pkt_hdr_pkt_len;     // Packet length

    // Packet data field
    uint32_t pkt_dat_fld_sec_hdr_t_fld_sec;  // T-field seconds
    uint16_t pkt_dat_fld_sec_hdr_t_fld_msec; // T-field milliseconds
    uint8_t  pkt_dat_fld_sec_hdr_p_fld;      // P-field
    uint32_t pkt_dat_fld_app_dat;            // Application data
    uint16_t pkt_dat_fld_pkt_err_cnt;        // Packet error control

    // Packet sub-fields (converted) 
    // Packet header:
    uint8_t  pkt_id_vrs;           // Version
    uint8_t  pkt_id_typ;           // Type
    uint8_t  pkt_id_sec_hdr_flg;   // Secondary header flag
    uint16_t pkt_id_apid;          // Application I.D.
    uint8_t  pkt_seq_cnt_grp_flg;  // Group flag
    uint16_t pkt_seq_cnt_pkt_name; // Packet name
    uint16_t pkt_len;              // Packet length (in octets - 1)

    // Packet data field packet secondary header:
    uint32_t pkt_t_fld_sec;   // T-field seconds (Unix timestamp)  
    uint16_t pkt_t_fld_msec;  // T-field milliseconds (Unix timestamp)
    uint8_t  pkt_p_fld_ext;   // P-field extension
    uint8_t  pkt_p_fld_id;    // P-field I.D.
    uint8_t  pkt_p_fld_bas;   // P-field basic time size (in octets - 1)
    uint8_t  pkt_p_fld_frc;   // P-field fractional time size (in octets - 1)

    // Packet data field application data:
    uint8_t  pkt_app_dat_atc_flg; // Absolutely timed command flag
    uint32_t pkt_app_dat_cmd_arg; // Command arguments

    // Packet data field error control:
    uint16_t pkt_err_cnt;

    // Task synchronize with proc_telecmd_pkt_task
    // (tell task that it is now ready to receive packets)
    rt_printf("%d (PROC_TELECMD_PKT_TASK)"
        " Ready to receive and process telecommand packets\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&telecmd_pkt_sem);

    // Infinite loop to receive telecommand packets from receive packets task
    // and process the packets into telecommand transfer frames. If packet
    // is valid, then the command transfer frame is sent to command executor 
    // task via message queue.
    while(1) {
        // Read telecommand packets from message queue:
        ret_val = rt_queue_read(&telecmd_pkt_msg_queue,&telecmd_pkt_buf,\
            TELECMD_PKT_SIZE,TM_INFINITE); // Will wait infinite amount of
                                           // time for message
        // Check success:
        if (ret_val == TELECMD_PKT_SIZE) {
            // Print:
            rt_printf("%d (PROC_TELECMD_PKT_TASK)"
                    " Received telecommand packet from receiver"
                    " task\n",time(NULL));
        } else {
            // NEED ERROR HANDLING
        }

        // Parse packet into packet fields (raw data):
        memcpy(&pkt_hdr_pkt_id,telecmd_pkt_buf,2);                    // 2 bytes of buffer (offset 0)
        memcpy(&pkt_hdr_pkt_seq_cnt,telecmd_pkt_buf+2,2);             // 2 bytes of buffer (offset 2)
        memcpy(&pkt_hdr_pkt_len,telecmd_pkt_buf+4,2);                 // 2 bytes of buffer (offset 4)
        memcpy(&pkt_dat_fld_sec_hdr_t_fld_sec,telecmd_pkt_buf+6,4);   // 4 byte of buffer (offset 6)
        memcpy(&pkt_dat_fld_sec_hdr_t_fld_msec,telecmd_pkt_buf+10,2); // 2 byte of buffer (offset 10)
        memcpy(&pkt_dat_fld_sec_hdr_p_fld,telecmd_pkt_buf+13,1);      // 1 byte of buffer (offset 13)
        memcpy(&pkt_dat_fld_app_dat,telecmd_pkt_buf+14,4);            // 4 bytes of buffer (offset 14)
        memcpy(&pkt_dat_fld_pkt_err_cnt,telecmd_pkt_buf+18,2);        // 2 bytes of buffer (offset 18)

        // Convert packet fields (raw data) to packet sub-fields (values)
        // Check for expected values of sub-fields to validate telecommand
        // packet. If a value does not match what is expected, ignore 
        // telecommand packet.

        // Packet I.D. Version:
        pkt_id_vrs = pkt_hdr_pkt_id & 0x07; // Mask to keep bits 0-2

        // Packet I.D. Type:
        pkt_id_typ = pkt_hdr_pkt_id & 0x08; // Mask to keep bit 3
        pkt_id_typ = pkt_id_typ >> 3;       // Shift right by 3 bits

        // Packet I.D. Secondary Header Flag:
        pkt_id_sec_hdr_flg = pkt_hdr_pkt_id & 0x10;   // Mask to keep bit 4
        pkt_id_sec_hdr_flg = pkt_id_sec_hdr_flg >> 4; // Shift right by 
                                                      // 4 bits

        // Packet I.D. APID:
        pkt_id_apid = pkt_hdr_pkt_id & 0xFFE0; // Mask to keep bits 5-15
        pkt_id_apid = pkt_id_apid >> 5;        // Shift right by 5 bits

        // Packet Sequence Control Grouping Flag:
        pkt_seq_cnt_grp_flg = pkt_hdr_pkt_seq_cnt & 0x3; // Mask to keep
                                                         // bits 0-1

        // Packet Sequence Control Packet Name:
        pkt_seq_cnt_pkt_name = pkt_hdr_pkt_seq_cnt & 0xFFFC; // Mask to keep
                                                             // bits 2-15
        pkt_seq_cnt_pkt_name = pkt_hdr_pkt_seq_cnt >> 2;     // Shift right by
                                                             // 2 bits

        // Packet Header Packet Length
        pkt_len = pkt_hdr_pkt_len;

        // Packet Data Field Packet Secondary Header T-Fields:
        pkt_t_fld_sec = pkt_dat_fld_sec_hdr_t_fld_sec;
        pkt_t_fld_msec = pkt_dat_fld_sec_hdr_t_fld_msec;

        // Packet Secondary Header P-Field Extension:
        pkt_p_fld_ext = pkt_dat_fld_sec_hdr_p_fld & 0x0; // Mask to keep bit 0

        // Packet Secondary Header P-Field Identification:
        pkt_p_fld_id = pkt_dat_fld_sec_hdr_p_fld & 0xE; // Mask to keep
                                                        // bits 1-3
        pkt_p_fld_id = pkt_p_fld_id >> 1;               // Shift right by 1 bit

        // Packet Secondary Header P-Field Variation:
        pkt_p_fld_bas = pkt_dat_fld_sec_hdr_p_fld & 0x30; // Mask to keep
                                                          // bit 4-5
        pkt_p_fld_bas = pkt_p_fld_bas >> 4;               // Shift right
                                                          // by 4 bits

        // Packet Secondary Header P-Field Variation:
        pkt_p_fld_frc = pkt_dat_fld_sec_hdr_p_fld & 0xC0; // Mask to keep
                                                          // bit 6-7
        pkt_p_fld_frc = pkt_p_fld_frc >> 6;               // Shift right by
                                                          // 5 bits

         // Packet Data Field Application Data
        pkt_app_dat_atc_flg = pkt_dat_fld_app_dat & 0x01; // Mask to keep bit 0

        pkt_app_dat_cmd_arg = pkt_dat_fld_app_dat & 0xFFFFFFFE; // Mast to keep
                                                                // bits 1-7
        pkt_app_dat_cmd_arg = pkt_app_dat_cmd_arg >> 1;         // Shift right
                                                                // by 1 bit

        // Packet Data Field Packet Error Control:
        pkt_err_cnt = pkt_dat_fld_pkt_err_cnt;

        // Print:
        rt_printf("%d (PROC_TELECMD_PKT_TASK) Telecommand packet is valid;"
            " creating command transfer frame\n",time(NULL));

        // Copy APID, packet name, execution time, ATC flag, arguments into
        // command transfer frame:
        memcpy(cmd_xfr_frm_buf+0,&pkt_id_apid,2);
        memcpy(cmd_xfr_frm_buf+2,&pkt_seq_cnt_pkt_name,2);
        memcpy(cmd_xfr_frm_buf+4,&pkt_t_fld_sec,4);
        memcpy(cmd_xfr_frm_buf+10,&pkt_t_fld_msec,2);
        memcpy(cmd_xfr_frm_buf+12,&pkt_app_dat_atc_flg,1);
        memcpy(cmd_xfr_frm_buf+13,&pkt_app_dat_cmd_arg,4);

        // Print:
        rt_printf("%d (PROC_TELECMD_PKT_TASK) Command transfer frame"
            " created\n",time(NULL));

        // Send command transfer frame to command executor task
        // via message que:
        ret_val = rt_queue_write(&cmd_xfr_frm_msg_queue,&cmd_xfr_frm_buf,\
            CMD_XFR_FRM_SIZE,Q_NORMAL); // Append message to queue

        // Check success:
        if (ret_val > 0) {
            // Print:
            rt_printf("%d (PROC_TELECMD_PKT_TASK) Command transfer frame sent"
            " to command executor task\n",time(NULL));
        } else {
            // NEED ERROR HANDLING
        }

        /*
        // Print results:
        rt_printf("Packet Header\n");
        rt_printf("  Packet I.D.\n");
        rt_printf("      Version                 : %u\n",pkt_id_vrs);
        rt_printf("      Type                    : %u\n",pkt_id_typ);
        rt_printf("      Secondary Header Flag   : %u\n",pkt_id_sec_hdr_flg);
        rt_printf("      APID                    : %u\n",pkt_id_apid);
        rt_printf("  Packet Sequence Control\n");
        rt_printf("      Grouping Flags          : %u\n",pkt_seq_cnt_grp_flg);
        rt_printf("      Packet Name             : %u\n",pkt_seq_cnt_pkt_name);
        rt_printf("  Packet Length\n");
        rt_printf("      Packet Length           : %u\n",pkt_len);
        rt_printf("Packet Data Field\n");
        rt_printf("  Packet Secondary Header\n");
        rt_printf("      T-Field Sec             : %u\n",pkt_t_fld_sec);
        rt_printf("      T-Field mSec            : %u\n",pkt_t_fld_msec);
        rt_printf("      P-Field Ext             : %u\n",pkt_p_fld_ext);
        rt_printf("      P-Field I.D.            : %u\n",pkt_p_fld_id);
        rt_printf("      P-Field Basic Octets    : %u\n",pkt_p_fld_bas);
        rt_printf("      P-Field Fraction Octets : %u\n",pkt_p_fld_frc);
        rt_printf("  Application Data\n");
        rt_printf("      ATC Flag                : %u\n",pkt_app_dat_atc_flg);
        rt_printf("      Command Argument        : %u\n",pkt_app_dat_cmd_arg);
        rt_printf("  Packet Error Control\n");
        rt_printf("      Packet Error Control    : %u\n",pkt_err_cnt);
        rt_printf("\n");
        */
    }

    // This will never be reached:
    return;
}