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

// Task function declarations:
void rx_telecmd_pkt(void* arg);   // Receive telecommand packet from uplink
                                  // serial port
void proc_telecmd_pkt(void* arg); // Process telecommand packet
void exec_cmd(void* arg);         // Execute command
void sched_cmd(void* arg);        // Command scheduler
void cmd_sw(void* arg);           // Execute software command
void cmd_img(void* arg);          // Execute imaging command
void cmd_mdq(void* arg);          // Execute magnetometer DAQ command
void cmd_ers(void* arg);          // Execute electrical relay switch command