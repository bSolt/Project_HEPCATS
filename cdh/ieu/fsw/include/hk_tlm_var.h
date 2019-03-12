///////////////////////////////////////////////////////////////////////////////
//
// Housekeeping (HK) Telemetry Variables
//
// Housekeeping (HK) Telemetry declarations
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: February 6, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Variable declaration:
extern uint8_t  val_telecmd_pkt_cnt;     // Valid telecommand packet counter
extern uint8_t  inv_telecmd_pkt_cnt;     // Invalid telecommand packet counter
extern uint8_t  rx_telecmd_pkt_cnt;      // Received telecommand packet count
extern uint8_t  val_cmd_cnt;             // Valid command counter
extern uint8_t  inv_cmd_cnt;             // Invalid command counter
extern uint8_t  cmd_exec_suc_cnt;        // Commands executed successfully counter
extern uint8_t  cmd_exec_err_cnt;        // Commands not executed (error) counter
extern uint16_t tlm_pkt_xfr_frm_seq_cnt; // Packet sequence count
extern uint16_t acq_img_cnt;             // Acquired images count
extern uint8_t  img_acq_prog_flag;       // Image acquisition in progress flag
extern uint8_t  mdq_scan_state;          // Magnetometer DAQ scanning state
extern uint8_t  ers_rly_swtch_state;     // Electrical relay switch state
extern uint8_t  flt_tbl_mode;            // Filter table mode
extern uint16_t img_accpt_cnt;           // Accepted images (from IPS) count
extern uint16_t img_rej_cnt;             // Rejected images (from IPS) count
extern uint32_t next_img_acq_tm;         // Next image acquisition time
extern uint32_t next_atc_tm;             // Next absolutely timed command time
extern uint8_t  pbk_prog_flg;            // Playback in progress flag
extern uint32_t sys_tm;                  // System time