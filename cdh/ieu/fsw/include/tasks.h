///////////////////////////////////////////////////////////////////////////////
//
// Task Header
//
// Task variable and function declarations
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

// Task declarations:
extern RT_TASK rx_telecmd_pkt_task;
extern RT_TASK proc_telecmd_pkt_task;

// Task function declarations:
void rx_telecmd_pkt(void* arg);   // Receive telecommand packet
void proc_telecmd_pkt(void* arg); // Process telecommand packet