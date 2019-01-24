///////////////////////////////////////////////////////////////////////////////
//
// Message Queues Header
//
// Xenomai message queue variable declarations
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
extern RT_QUEUE telecmd_pkt_msg_queue; // For telecommand packets
                                       // (rx_telecmd_pkt_task
                                       //  --> proc_telecmd_pkt_task)
extern RT_QUEUE cmd_xfr_frm_msg_queue; // For command transfer frames
                                       // (proc_telecmd_pkt_task/cmd_sched_task
                                       //  --> exec_cmd_task)
