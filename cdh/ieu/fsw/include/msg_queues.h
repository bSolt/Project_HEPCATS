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
extern RT_QUEUE daq_src_dat_msg_queue; // For magnetometer DAQ source data
                                       // (read_usb_daq_task
                                       // --> crt_tlm_pkt_task)
extern RT_QUEUE img_src_dat_msg_queue; // For imaging source data
                                       // (read_usb_im_task
                                       // --> crt_tlm_pkt_task)
extern RT_QUEUE flt_tbl_msg_queue;     // For telemetry packet transfer frames
                                       // (crt_tlm_pkt_task --> flt_tbl_task)
extern RT_QUEUE tx_tlm_pkt_msg_queue;  // For telemetry packets transfer frames
                                       // (flt_tbl_task/rtrv_file_task
                                       // --> tx_tlm_pkt_task)
extern RT_QUEUE crt_file_msg_queue;    // For telemetry packet transfer frames
                                       // (flt_tbl_task --> crt_file_task)