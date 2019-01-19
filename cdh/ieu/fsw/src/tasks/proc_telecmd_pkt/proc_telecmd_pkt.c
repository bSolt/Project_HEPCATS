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
#define TELECMD_PKT_SIZE    20 // Telecommand packet size in bytes
#define CMD_XFR_FRM_SIZE    15 // Command transfer frame size in bytes
#define EXP_PKT_VER          0 // Expected telecommand packet version value
#define EXP_PKT_TYP          1 // Expected telecommand packet type value
#define EXP_PKT_SEC_HDR_FLG  1 // Expected telecommand secondary header
                               // flag value
#define EXP_PKT_APID_RNG  2047 // Expected telecommand packet APID range (max)
#define EXP_PKT_GRP_FLG_RNG  3 // Expected telecommand packet group flag
                               // range (max)
#define EXP_PKT_NAME_RNG 16383 // Expected telecommand packet name range
#define EXP_PKT_LEN         13 // Expected telecommand packet length - 1
#define EXP_PKT_ATC_FLG_RNG  1 // Expected telecommand packet ATC flag
                               // range (max)
#define EXP_PKT_ERR_CNT  65535 // Expected telecommand packet error control
#define EXP_PKT_CMD_ARG_RNG \
    2147483647                 // Expected telecommand packet argument range
#define CMD_EXEC_TM_TOL    -20 // Telecommand packet execution time tolerance
                               // (will not except packet if execution time is
                               //  in this past by more than this many seconds)
#define EXP_PKT_P_EXT        0 // Expected telecommand packet p-field 
                               // extension flag
#define EXP_PKT_P_ID         2 // Expected telecommand packet p-field
                               // identification
#define EXP_PKT_P_BAS_LEN    3 // Expected telecommand packet p-field basic
                               // time length in octets - 1
#define EXP_PKT_P_FRC_LEN    2 // Expected telecommand packet p-field basic
                               // time length in octets - 1

// Message queue definitions:
RT_QUEUE telecmd_pkt_msg_queue; // For telecommand packets
                                // (rx_telecmd_pkt_task
                                //  --> proc_telecmd_pkt_task)
RT_QUEUE cmd_xfr_frm_msg_queue; // For command transfer frames
                                // (proc_telecmd_pkt_task/cmd_sched
                                //  --> exec_cmd_task)

// Semaphore definitions:
RT_SEM telecmd_pkt_sem; // For rx_telecmd_pkt_task and proc_telecmd_pkt_task
                        // task synchronization

void proc_telecmd_pkt(void) {
    // Print:
    rt_printf("%d (PROC_TELECMD_PKT_TASK) Task started\n",time(NULL));

    // Task synchronize with cmd_exec task
    // (Wait for task to be ready to receive frames)
    rt_printf("%d (PROC_TELECMD_PKT_TASK) Waiting for execute command"
        " task to be ready\n",time(NULL));

    // Wait for signal:
    rt_sem_p(&cmd_xfr_frm_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (PROC_TELECMD_PKT_TASK)"
        " Execute command task is ready; continuing\n",time(NULL));

    // Definitions and initializations:
    int8_t ret_val; // Function return value

    uint8_t val_telecmd_pkt_cnt = 0; // Valid telecommand packet counter
    uint8_t inv_telecmd_pkt_cnt = 0; // Invalid telecommand packet counter

    char  telecmd_pkt_buf[TELECMD_PKT_SIZE]; // Buffer for telecommand packet
    char cmd_xfr_frm_buf[CMD_XFR_FRM_SIZE]; // Buffer for command transfer
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

    // Task synchronize with rx_telecmd_pkt task
    // (tell task that it is now ready to receive packets)
    rt_printf("%d (PROC_TELECMD_PKT_TASK)"
        " Ready to receive and process telecommand packets\n",time(NULL));

    // Signal task to continue executing:
    rt_sem_v(&telecmd_pkt_sem);

    // Infinite loop to receive telecommand packets from receive packets task
    // and process the packets into telecommand transfer frames. If packet
    // is valid, then the command transfer frame is sent to command execuetor
    // task via message queue. If it is not valid, it is not processed and a
    // command transfer frame is not created. This is done using goto and
    // labels: read_telecmd_pkt and inv_pkt. The read_telecmd_pkt is the "top
    // of the loop" label. inv_pkt label is only reached if one data field is
    // not what is expected (either exact value or range).
    while(1) {
        // "Top of the loop" Label:
        read_telecmd_pkt:

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
            // Print:
            rt_printf("%d (PROC_TELECMD_PKT_TASK)"
                " Error reading telecommand packet from receiver"
                " task\n",time(NULL));
            // NEED ERROR HANDLING
        }

        // Parse packet telecommand into packet fields (raw data):
        memcpy(&pkt_hdr_pkt_id,telecmd_pkt_buf,2);
        memcpy(&pkt_hdr_pkt_seq_cnt,telecmd_pkt_buf+2,2);
        memcpy(&pkt_hdr_pkt_len,telecmd_pkt_buf+4,2);
        memcpy(&pkt_dat_fld_sec_hdr_t_fld_sec,telecmd_pkt_buf+6,4);
        memcpy(&pkt_dat_fld_sec_hdr_t_fld_msec,telecmd_pkt_buf+10,2);
        memcpy(&pkt_dat_fld_sec_hdr_p_fld,telecmd_pkt_buf+13,1);
        memcpy(&pkt_dat_fld_app_dat,telecmd_pkt_buf+14,4);
        memcpy(&pkt_dat_fld_pkt_err_cnt,telecmd_pkt_buf+18,2);

        // Convert packet fields (raw data) to packet sub-fields (values)
        // Check for expected values of data fields to validate telecommand
        // packet. If a value does not match what is expected (either range
        // or exact value, go to inv_pkt label to ignore telecommand packet
        // and skip further processing.

        // Print:
        rt_printf("%d (PROC_TELECMD_PKT_TASK) Processing telecommand"
            " packet\n",time(NULL));

        // Packet I.D. version:
        pkt_id_vrs = pkt_hdr_pkt_id & 0x07; // Mask to keep bits 0-2

        // Check packet version for expected value. If true, go to invalid
        // packet label:
        if ((pkt_id_vrs != EXP_PKT_VER)) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet I.D. type:
        pkt_id_typ = pkt_hdr_pkt_id & 0x08; // Mask to keep bit 3
        pkt_id_typ = pkt_id_typ >> 3;       // Shift right by 3 bits

        // Check packet type for expected value. If true, go to invalid
        // packet label:
        if ((pkt_id_typ != EXP_PKT_TYP)) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet I.D. secondary header flag:
        pkt_id_sec_hdr_flg = pkt_hdr_pkt_id & 0x10;   // Mask to keep bit 4
        pkt_id_sec_hdr_flg = pkt_id_sec_hdr_flg >> 4; // Shift right by
                                                      // 4 bits

        // Check packet secondary header flag for expected value. If true,
        // go to invalid packet label:
        if ((pkt_id_sec_hdr_flg != EXP_PKT_SEC_HDR_FLG)) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet I.D. APID:
        pkt_id_apid = pkt_hdr_pkt_id & 0xFFE0; // Mask to keep bits 5-15
        pkt_id_apid = pkt_id_apid >> 5;        // Shift right by 5 bits

        // Check packet APID range. If true, go to invalid packet label:
        if (pkt_id_apid > EXP_PKT_APID_RNG) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet sequence control grouping flag:
        pkt_seq_cnt_grp_flg = pkt_hdr_pkt_seq_cnt & 0x3; // Mask to keep
                                                         // bits 0-1

        // Check packet APID range. If true, go to invalid packet label:
        if (pkt_seq_cnt_grp_flg > EXP_PKT_GRP_FLG_RNG) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet sequence control packet name:
        pkt_seq_cnt_pkt_name = pkt_hdr_pkt_seq_cnt \
            & 0xFFFC; // Mask to keep
                      // bits 2-15
        pkt_seq_cnt_pkt_name = pkt_hdr_pkt_seq_cnt >> 2; // Shift right
                                                         // by 2 bits

        // Check packet name range. If true, go to invalid packet label:
        if (pkt_seq_cnt_pkt_name > EXP_PKT_NAME_RNG) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet header packet length
        pkt_len = pkt_hdr_pkt_len;

        // Check packet name range. If true, go to invalid packet label:
        if (pkt_len > EXP_PKT_LEN) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet data field packet secondary header T-fields:
        pkt_t_fld_sec = pkt_dat_fld_sec_hdr_t_fld_sec;
        pkt_t_fld_msec = pkt_dat_fld_sec_hdr_t_fld_msec;

        // Check packet T-field for a command execution time in the past by
        // a specificed tolerance. Will not accept packet if execution time is
        // in this past by more than this many seconds. If true, go to 
        // invalid packet label:
        if ((pkt_t_fld_sec - time(NULL)) < CMD_EXEC_TM_TOL) {
            // Print:
            rt_printf("%d (PROC_TELECMD_PKT_TASK) Command execution time is" 
                " in the past by %d seconds \n",time(NULL),(pkt_t_fld_sec - time(NULL)));

            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet secondary header P-field extension:
        pkt_p_fld_ext = pkt_dat_fld_sec_hdr_p_fld & 0x0; // Mask to keep bit 0

        // Check packet P-field extension flag for expected value. If true, 
        // go to invalid packet label:
        if (pkt_p_fld_ext != EXP_PKT_P_EXT) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet secondary header P-field identification:
        pkt_p_fld_id = pkt_dat_fld_sec_hdr_p_fld & 0xE; // Mask to keep
                                                        // bits 1-3
        pkt_p_fld_id = pkt_p_fld_id >> 1;               // Shift right by 1 bit

        // Check packet P-field extension identification for expected value. 
        // If true, go to invalid packet label:
        if (pkt_p_fld_id != EXP_PKT_P_ID) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet secondary header P-field basic time octets:
        pkt_p_fld_bas = pkt_dat_fld_sec_hdr_p_fld & 0x30; // Mask to keep
                                                          // bit 4-5
        pkt_p_fld_bas = pkt_p_fld_bas >> 4;               // Shift right
                                                          // by 4 bits

        // Check packet p-field basic time octets for expected value.
        // If true, go to invalid packet label:
        if (pkt_p_fld_bas != EXP_PKT_P_BAS_LEN) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet secondary header P-field fractional time octets:
        pkt_p_fld_frc = pkt_dat_fld_sec_hdr_p_fld & 0xC0; // Mask to keep
                                                          // bit 6-7
        pkt_p_fld_frc = pkt_p_fld_frc >> 6;               // Shift right by
                                                          // 5 bits

        // Check packet p-field basic time octets for expected value.
        // If true, go to invalid packet label:
        if (pkt_p_fld_frc != EXP_PKT_P_FRC_LEN) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet data field application data ATC flag:
        pkt_app_dat_atc_flg = pkt_dat_fld_app_dat & 0x01; // Mask to keep bit 0

        // Check packet error control for expected value. If true, go
        // to invalid packet label:
        if (pkt_app_dat_atc_flg > EXP_PKT_ATC_FLG_RNG) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet data field application data command arguments:
        pkt_app_dat_cmd_arg = pkt_dat_fld_app_dat & 0xFFFFFFFE; // Mast to keep
                                                                // bits 1-7
        pkt_app_dat_cmd_arg = pkt_app_dat_cmd_arg >> 1;         // Shift right
                                                                // by 1 bit

        // Check packet error control for expected value. If true, go
        // to invalid packet label:
        if (pkt_app_dat_cmd_arg > EXP_PKT_CMD_ARG_RNG) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Packet data field packet error control:
        pkt_err_cnt = pkt_dat_fld_pkt_err_cnt;

        // Check packet error control for expected value. If true, go
        // to invalid packet label:
        if (pkt_err_cnt != EXP_PKT_ERR_CNT) {
            // Goto invalid packet:
            goto inv_pkt;
        }

        // Print:
        rt_printf("%d (PROC_TELECMD_PKT_TASK) Telecommand packet is valid;"
            " creating command transfer frame\n",time(NULL));

        // Build command transfer frame:
        // (APID, packet name, execution time, ATC flag, and command arguments)
        memcpy(cmd_xfr_frm_buf+0,&pkt_id_apid,2);
        memcpy(cmd_xfr_frm_buf+2,&pkt_seq_cnt_pkt_name,2);
        memcpy(cmd_xfr_frm_buf+4,&pkt_app_dat_atc_flg,1);
        memcpy(cmd_xfr_frm_buf+5,&pkt_t_fld_sec,4);
        memcpy(cmd_xfr_frm_buf+9,&pkt_t_fld_msec,2);
        memcpy(cmd_xfr_frm_buf+11,&pkt_app_dat_cmd_arg,4);

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
            rt_printf("%d (PROC_TELECMD_PKT_TASK) Command transfer frame" 
            " sent to execute command task\n",time(NULL));
        } else {
            // Print:
            rt_printf("%d (PROC_TELECMD_PKT_TASK) Error sending command"
                " transfer frame to execute command task\n",time(NULL));
            // NEED ERROR HANDLING
        }

        // Increase counter:
        ++val_telecmd_pkt_cnt;

        // Go to "Top of the loop" label to read another telecommand packet
        // for processing and skip inv_pkt label:
        goto read_telecmd_pkt;

        // Invalid packet label. This label is only reached if telecommand
        // packet has at least one data field that is not expected (either
        // in value or range). Packet is ignored and command transfer frame
        // is not created.
        // ----------------------------------------------------------------- //
        // Invalid packet label:
        inv_pkt:

        // Print:
        rt_printf("%d (PROC_TELECMD_PKT_TASK) Telecommand packet is"
            " invalid; packet ignored\n",time(NULL));

        // Increase counter:
        ++inv_telecmd_pkt_cnt;

    }

    // This will never be reached:
    return;
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
