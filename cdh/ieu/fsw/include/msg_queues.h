///////////////////////////////////////////////////////////////////////////////
//
// Message Queue Header
//
// Message queue declarations
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

// Message queue declarations:
extern RT_QUEUE telecmd_msg_queue; // For command transfer packets
                                   // (rx_telecmd_pkt_task 
                                   //                 --> proc_telecmd_pkt_task)