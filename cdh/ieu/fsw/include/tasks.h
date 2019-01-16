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
extern RT_TASK exec_cmd_task; 		  // Execute command

// Task function declarations:
void rx_telecmd_pkt(void* arg);   // Receive telecommand packet from uplink
                                  // serial port
void proc_telecmd_pkt(void* arg); // Process telecommand packet
void exec_cmd(void* arg);         // Execute command
			