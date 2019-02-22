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
extern RT_QUEUE flt_tbl_msg_queue;     // For telemetry packet transfer frames
                                       // (read_usb/get_hk_tlm
                                       //  --> flt_tbl_task)
extern RT_QUEUE tx_tlm_pkt_msg_queue;  // For telemetry packets transfer frames
                                       // (flt_tbl_task/rtrv_file_task
                                       // --> tx_tlm_pkt_task)
extern RT_QUEUE crt_file_msg_queue;    // For telemetry packet transfer frames
                                       // (flt_tbl_task --> crt_file_task)