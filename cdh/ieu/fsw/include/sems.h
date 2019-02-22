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
extern RT_SEM rx_telecmd_pkt_sem;   // For rx_telecmd_pkt_task and 
                                    // proc_telecmd_pkt_task task 
                                    // synchronization
extern RT_SEM proc_telecmd_pkt_sem; // For proc_telecmd_pkt_task and exec_cmd 
                                    // task synchronization
extern RT_SEM cmd_sched_sem;        // For exec_cmd and cmd_sched task
                                    // synchronization
extern RT_SEM cmd_sw_sem;           // For exec_cmd and cmd_sw task 
                                    // synchronization
extern RT_SEM cmd_img_sem;          // For exec_cmd and cmd_img task 
                                    // synchronization
extern RT_SEM cmd_mdq_sem;          // For exec_cmd and cmd_mdq task 
                                    // synchronization
extern RT_SEM cmd_ers_sem;          // For exec_cmd and cmd_ers task 
                                    // synchronization
extern RT_SEM flt_tbl_sem;          // For flt_tbl_task, read_usb, and
                                    // get_hk_tlm task synchronization
extern RT_SEM tx_tlm_pkt_sem;       // For tx_tlm_pkt_task, flt_tbl_task
                                    // and rtrv_file_task synchronization
extern RT_SEM crt_file_sem;         // For crt_file_task and flt_tbl_task
                                    // synchronization
extern RT_SEM rtrv_file_sem;        // For rtrv_file_task and cmd_sw_task
                                    // synchronization
extern RT_SEM new_img_sem;          // For run_cam_sgl and read_usb_img task
                                    // synchronization     