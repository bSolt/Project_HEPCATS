///////////////////////////////////////////////////////////////////////////////
//
// Xenomai Task Header
//
// Xenomai task variable and function declarations
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: January 7, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Task variable declarations:
extern RT_TASK rx_telecmd_pkt_task;   // Receive telecommand packet from uplink
                                      // serial port
extern RT_TASK proc_telecmd_pkt_task; // Process telecommand packet
extern RT_TASK exec_cmd_task;         // Execute command
extern RT_TASK sched_cmd_task;        // Command scheduler
extern RT_TASK cmd_sw_task;           // Execute software command
extern RT_TASK cmd_img_task;          // Execute imaging command
extern RT_TASK cmd_mdq_task;          // Execute magnetometer DAQ command
extern RT_TASK cmd_ers_task;          // Execute electrical relay switch
                                      // command
extern RT_TASK crt_tlm_pkt_task;      // Create telemetry packet
extern RT_TASK cmd_img_task;          // Execute imaging command
extern RT_TASK cmd_mdq_task;          // Execute magnetometer DAQ command
extern RT_TASK flt_tbl_task;          // (Telemetry) Filter table
extern RT_TASK tx_tlm_pkt_task;       // Transmit telemetry packet to downlink
                                      // serial port
extern RT_TASK crt_file_task;         // Create file from telemetry packet
                                      // transfer frame
extern RT_TASK rtrv_file_task;        // Retrieve file for downlink
extern RT_TASK get_hk_tlm_task;	      // Get housekeeping telemetry

// Task function declarations:
void rx_telecmd_pkt(void* arg);    // Receive telecommand packet from uplink
                                   // serial port
void proc_telecmd_pkt(void* arg);  // Process telecommand packet
void exec_cmd(void* arg);          // Execute command
void sched_cmd(void* arg);         // Command scheduler
void cmd_sw(void* arg);            // Execute software command
void cmd_img(void* arg);           // Execute imaging command
void cmd_mdq(void* arg);           // Execute magnetometer DAQ command
void cmd_ers(void* arg);           // Execute electrical relay switch command
void crt_tlm_pkt(void* arg);       // Create telemetry packet 
void read_mdq(void* arg);          // Read magnetometer DAQ
void read_img(void* arg);          // Read imaging
void flt_tbl(void* arg);           // (Telemetry) Filter table
void tx_tlm_pkt(void* arg);        // Transmit telemetry packet to downlink
                                   // serial port
void crt_file(void* arg);          // Create file from telemetry packet
                                   // transfer frame
void rtrv_file(void* arg);         // Retrieve file for downlink
void get_hk_tlm(void* arg);	       // Get housekeeping telemetry