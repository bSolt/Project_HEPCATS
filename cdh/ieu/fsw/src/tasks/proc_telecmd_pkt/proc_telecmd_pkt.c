///////////////////////////////////////////////////////////////////////////////
//
// Process Telecommand Packet
//
// Task responsible for processing telecommand packets received via message 
// queue from receive telecommand packet task. Packets are processed to 
// recover packet
//     - APID
//     - Packet name
//     - Execution time
//     - ATC flag
//     - Arguments
// These values are then packaged into a command transfer packets and sent 
// to the command executor task via message queue.
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
// Created: January 10, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <errno.h>   // Error number definitions
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types

// Xenomai libraries:
#include <alchemy/task.h>  // Task management service
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations

// Message queue definitions:
RT_QUEUE telecmd_msg_queue; // For command transfer packets
                            // (rx_telecmd_pkt_task --> proc_telecmd_pkt_task)

// Semaphore definitions:
RT_SEM telecmd_pkt_sem; // For rx_telecmd_pkt_task and proc_telecmd_pkt_task
                        // task synchronization

void proc_telecmd_pkt(void){
    // Print:
    rt_printf("%d (PROC_TELECMD_PKT_TASK) Task started\n",time(NULL));

    // Initialize:
    int8_t ret_val;                // Function return value
    uint8_t telecmd_pkt_size = 17; // Packet size in bytes

    // Define buffer:
    char buf[telecmd_pkt_size];

    // Task synchronize with proc_telecmd_pkt_task
    // (Tell task that it is now ready to receive packets)
    rt_printf("%d (PROC_TELECMD_PKT_TASK)"
        " Ready to receive and process telecommand packets\n",time(NULL));

    // Signal:
    rt_sem_v(&telecmd_pkt_sem);

    // Infinite loop to receive telecommand packets from receive 
    // packets task:
    while(1) {
        // Read telecommand packets from message queue:
        // (returns size of message in bytes)
        ret_val = rt_queue_read(&telecmd_msg_queue,&buf,\
            telecmd_pkt_size,TM_INFINITE); // Will wait infinite amount of
                                           // time for message

        // Check success:
        if (ret_val == telecmd_pkt_size) {
            // Print:
            rt_printf("%d (PROC_TELECMD_PKT_TASK)"
                    " Received telecommand packet\n",time(NULL));
        } else {
            // NEED ERROR HANDLING
        }
    }

    // This will never be reached:
    return;
}