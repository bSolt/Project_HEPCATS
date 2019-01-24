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

// Create message queues and message pipes
void crt_msg_queues_pipes() {
    // Print:
    rt_printf("%d (STARTUP/CRT_MSG_QUEUES_PIPES)"
        " Creating message queues and pipes\n",time(NULL));
 
    // Create message queues:
    rt_queue_create(&telecmd_pkt_msg_queue,\
        "telecmd_pkt_msg_queue",20,10,Q_FIFO); // 20 bytes per message
                                               // Max 10 messages in queue
                                               // First in, first out
    // Create message queues:
    rt_queue_create(&cmd_xfr_frm_msg_queue,\
        "cmd_xfr_frm_msg_queue",17,10,Q_FIFO); // 17 bytes per message
                                               // Max 10 messages in queue
                                               // First in, first out

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

    // Print:
    rt_printf("%d (STARTUP/CRT_SEMS)"
        " Semaphores created\n",time(NULL));
}