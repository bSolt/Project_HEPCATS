///////////////////////////////////////////////////////////////////////////////
//
// Semaphores Header
//
// Xenomai semaphore variable declarations
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

// Semaphore declarations:
extern RT_SEM telecmd_pkt_sem; // For rx_telecmd_pkt_task and 
                               // proc_telecmd_pkt_task task synchronization
extern RT_SEM cmd_xfr_frm_sem; // For proc_telecmd_pkt_task and exec_cmd task
							   // synchronization