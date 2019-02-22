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
extern uint8_t  rx_telecmd_pkt_cnt; 	 // Received telecommand packet count
extern uint8_t  val_cmd_apid_cnt; 		 // Valid command counter
extern uint8_t  inv_cmd_apid_cnt; 		 // Invalid command counter
extern uint8_t  cmd_exec_suc_cnt; 		 // Commands executed successfully counter
extern uint8_t  cmd_exec_err_cnt; 		 // Commands not executed (error) counter
extern uint16_t tlm_pkt_xfr_frm_seq_cnt; // Packet sequence count