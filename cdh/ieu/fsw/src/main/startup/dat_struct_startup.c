///////////////////////////////////////////////////////////////////////////////
//
// Data Structures Startup
//
// Functions defined in this file create Xenomai data structures
// (message queues, pipes, semaphores, etc.) as apart of flight software
// initialization. 
//
// These functions are called by startup().
//
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - Xenomai 3 / Cobalt
//
// Input Arguments:
// - N/A
//
// Output Arguments:
// - N/A
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: January 12, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <time.h> // Time and date

// Xenomai libraries:
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services

// Header files:
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations

// Message queue definitions:
RT_QUEUE telecmd_pkt_msg_queue; // For command transfer frames
                                // (rx_telecmd_pkt_task 
                                //  --> proc_telecmd_pkt_task)
RT_QUEUE cmd_xfr_frm_msg_queue; // For command transfer frames
                                // (proc_telecmd_pkt_task/cmd_sched_task
                                //  --> exec_cmd_task)
RT_QUEUE daq_src_dat_msg_queue; // For magnetometer DAQ source data
                                // (read_usb_daq_task --> crt_tlm_pkt_task)
RT_QUEUE img_src_dat_msg_queue; // For imaging source data
                                // (read_usb_im_task --> crt_tlm_pkt_task)
RT_QUEUE flt_tbl_msg_queue;     // For telemetry packet transfer frames
                                // (crt_tlm_pkt_task --> flt_tbl_task)
RT_QUEUE tx_tlm_pkt_msg_queue;  // For telemetry packets
                                // (flt_tbl_task/rtrv_file_task
                                // --> tx_tlm_pkt_task)
RT_QUEUE crt_file_msg_queue;    // For telemetry packet transfer frames
                                // (flt_tbl_task --> crt_file_task)

// Semaphore definitions:
RT_SEM rx_telecmd_pkt_sem;   // For rx_telecmd_pkt_task and proc_telecmd_pkt_task
                             // task synchronization
RT_SEM proc_telecmd_pkt_sem; // For proc_telecmd_pkt_task and exec_cmd task
                             // synchronization
RT_SEM cmd_sched_sem;        // For exec_cmd and cmd_sched task
                             // synchronization
RT_SEM cmd_sw_sem;           // For exec_cmd and cmd_sw task synchronization
RT_SEM cmd_img_sem;          // For exec_cmd and cmd_img task synchronization 
RT_SEM cmd_mdq_sem;          // For exec_cmd and cmd_mdq task synchronization
RT_SEM cmd_ers_sem;          // For exec_cmd and cmd_mdq task synchronization
RT_SEM crt_tlm_pkt_sem;      // For crt_tlm_pkt_task and read_usb tasks
                             // synchronization
RT_SEM flt_tbl_sem;          // For flt_tbl_task and crt_tlm_pkt_task
                             // synchronization
RT_SEM tx_tlm_pkt_sem;       // For tx_tlm_pkt_task, flt_tbl_task, and 
                             // rtrv_file_task synchronization
RT_SEM crt_file_sem;         // For crt_file_task and flt_tbl_task
                             // synchronization
RT_SEM rtrv_file_sem;        // For rtrv_file_task and cmd_sw_task
                             // synchronization   

// Macro definitions:
#define TELECMD_PKT_QUEUE_NMSG 10 // Message queue limit
#define CMD_XFR_QUEUE_NMSG     10 // Message queue limit
#define DAQ_SRC_DAT_QUEUE_NMSG 10 // Message queue limit
#define IMG_SRC_DAT_QUEUE_NMSG 10 // Message queue limit
#define FLT_TBL_QUEUE_NMSG     10 // Message queue limit
#define TX_TLM_PKT_QUEUE_NMSG  10 // Message queue limit
#define CRT_FILE_QUEUE_NMSG    10 // Message queue limit

#define TELECMD_PKT_SIZE       20 // Telecommand packet size in bytes
#define CMD_XFR_FRM_SIZE       15 // Command transfer frame size in bytes
#define TLM_PKT_XFR_FRM_SIZE 1082 // Telemetry transfer frame size in bytes
#define DAQ_SRC_DAT_SIZE     1064 // Magnetometer DAQ source data message queue
                                  // size in bytes
#define IMG_SRC_DAT_SIZE     3000 // Imaging source data message queue
                                  // size in bytes (UPDATE)

// Create message queues and message pipes
void crt_msg_queues_pipes() {
    // Print:
    rt_printf("%d (STARTUP/CRT_MSG_QUEUES_PIPES)"
        " Creating message queues and pipes\n",time(NULL));
 
    // Create message queues:
    rt_queue_create(&telecmd_pkt_msg_queue,"telecmd_pkt_msg_queue",\
        TELECMD_PKT_SIZE*TELECMD_PKT_QUEUE_NMSG,TELECMD_PKT_QUEUE_NMSG,Q_FIFO);

    // Create message queues:
    rt_queue_create(&cmd_xfr_frm_msg_queue,"cmd_xfr_frm_msg_queue",\
        CMD_XFR_FRM_SIZE*CMD_XFR_QUEUE_NMSG,CMD_XFR_QUEUE_NMSG,Q_FIFO);

    // Create message queues:
    rt_queue_create(&daq_src_dat_msg_queue,"daq_src_dat_msg",\
        DAQ_SRC_DAT_SIZE*DAQ_SRC_DAT_QUEUE_NMSG,DAQ_SRC_DAT_QUEUE_NMSG,Q_FIFO);

    // Create message queues:
    rt_queue_create(&img_src_dat_msg_queue,"img_src_dat_msg_queue",\
        IMG_SRC_DAT_SIZE*IMG_SRC_DAT_QUEUE_NMSG,IMG_SRC_DAT_QUEUE_NMSG,Q_FIFO);

    // Create message queues:
    rt_queue_create(&flt_tbl_msg_queue,"flt_tbl_msg_queue",\
        TLM_PKT_XFR_FRM_SIZE*FLT_TBL_QUEUE_NMSG,FLT_TBL_QUEUE_NMSG,Q_FIFO);

    // Create message queues:
    rt_queue_create(&tx_tlm_pkt_msg_queue,"tx_tlm_pkt_msg_queue",\
        TLM_PKT_XFR_FRM_SIZE*TX_TLM_PKT_QUEUE_NMSG,TX_TLM_PKT_QUEUE_NMSG,Q_FIFO);

    // Create message queues:
    rt_queue_create(&crt_file_msg_queue,"crt_file_msg_queue",\
        TLM_PKT_XFR_FRM_SIZE*CRT_FILE_QUEUE_NMSG,CRT_FILE_QUEUE_NMSG,Q_FIFO);

    // Print:
    rt_printf("%d (STARTUP/CRT_MSG_QUEUES_PIPES)"
        " Message queues and pipes created\n",time(NULL));
}

// Create semaphores
void crt_sems() {
    // Print:
    rt_printf("%d (STARTUP/CRT_SEMS)"
        " Creating semaphores\n",time(NULL));

    // Create semaphores:
    rt_sem_create(&rx_telecmd_pkt_sem,"rx_telecmd_pkt_sem",0,S_FIFO);
    rt_sem_create(&proc_telecmd_pkt_sem,"proc_telecmd_pkt_sem",0,S_FIFO);
    rt_sem_create(&cmd_sched_sem,"cmd_sched_sem",0,S_FIFO);
    rt_sem_create(&cmd_sw_sem,"cmd_sw_sem",0,S_FIFO);
    rt_sem_create(&cmd_img_sem,"cmd_img_sem",0,S_FIFO);
    rt_sem_create(&cmd_mdq_sem,"cmd_mdq_sem",0,S_FIFO);
    rt_sem_create(&cmd_ers_sem,"cmd_ers_sem",0,S_FIFO);
    rt_sem_create(&cmd_ers_sem,"cmd_ers_sem",0,S_FIFO);
    rt_sem_create(&crt_tlm_pkt_sem,"crt_tlm_pkt_sem",0,S_FIFO);
    rt_sem_create(&flt_tbl_sem,"flt_tbl_sem",0,S_FIFO);
    rt_sem_create(&tx_tlm_pkt_sem,"tx_tlm_pkt_sem",0,S_FIFO);
    rt_sem_create(&crt_file_sem,"crt_file_sem",0,S_FIFO);
    rt_sem_create(&rtrv_file_sem,"rtrv_file_sem",0,S_FIFO);

    // Print:
    rt_printf("%d (STARTUP/CRT_SEMS)"
        " Semaphores created\n",time(NULL));
}