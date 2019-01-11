///////////////////////////////////////////////////////////////////////////////
//
// Receive (Rx) Telecommand Packet
//
// Task responsible for receiving telecommand packets from a serial port.
// Task first opens and configures a port then reads its for incoming
// packets. Once a packet is received, it is sent to the telecommand packet
// processor task via message queue.
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
// Created: January 7, 2019
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
#include <alchemy/queue.h> // Message pipe service
#include <alchemy/sem.h>   // Semaphore services

// Header files:
#include <open_port.h> // Open serial port function declaration
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations

// Message queue definitions:
RT_QUEUE telecmd_msg_queue; // For command transfer packets
                            // (rx_telecmd_pkt_task --> proc_telecmd_pkt_task)

// Semaphore definitions:
RT_SEM telecmd_pkt_sem; // For rx_telecmd_pkt_task and proc_telecmd_pkt_task
                        // task synchronization

// Macro definitions:
#define B2400 0000013 // Baud rate (as defined in terminos.h)

void rx_telecmd_pkt(void* arg){
    // Print:
    rt_printf("%d (RX_TELECMD_PKT_TASK) Task started\n",time(NULL));

    // Initialize:
    uint8_t debug_flg = 1;         // Debug flag
    int8_t fd;                     // File descriptor
    int8_t ret_val;                // Function return value
    uint8_t telecmd_pkt_size = 17; // Packet size in bytes
    uint8_t bytes_left = \
        telecmd_pkt_size;          // Bytes left to read
    uint8_t bytes_read = 0;        // Bytes read
    char buf[telecmd_pkt_size];    // Buffer

    // Define uplink serial port:
    char* port = "/dev/pts/5";

    // Open port:
    fd = open_port(port,B2400);

    // Check for success:
    if (fd > 0) {
        // Success:
        rt_printf("%d (RX_TELECMD_PKT_TASK)"
            " Uplink serial port opened and configured\n",time(NULL));
    } else {
        // Print:
        rt_printf("%d (RX_TELECMD_PKT_TASK)"
            " Uplink serial port not opened and configured\n",time(NULL));
        // NEED ERROR HANDLING
    }

    // Task synchronize with proc_telecmd_pkt_task
    // (Wait for task to be ready to receive and process packets)
    rt_printf("%d (RX_TELECMD_PKT_TASK)"
        " Waiting for telecommand packet processor\n",time(NULL));
    
    // Wait for signal:
    rt_sem_p(&telecmd_pkt_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (RX_TELECMD_PKT_TASK)"
        " Telecommand packet processor task ready."
        " Continuing...\n",time(NULL));

    // Infinite loop to read uplink serial port for telecommand packets:
    while(1) {
        // Loop to read full telecommand packet:
        while (bytes_left > 0 ) {
            // Read from port:
            // returns number of bytes read)
            ret_val = read(fd,&buf,bytes_left); 

            // Check to see if entire packet is received:
            if (ret_val > 0) {
                // Increment and decrement counters:
                bytes_read += ret_val;
                bytes_left -= ret_val;
            } else {
                // Print:
                rt_printf("%d (RX_TELECMD_PKT_TASK)"
                    "Error reading uplink port\n",time(NULL));
                // NEED ERROR HANDLING
            }
        }

        // Print:
        rt_printf("%d (RX_TELECMD_PKT_TASK)"
                    " Telecommand packet received\n",time(NULL));

        // Send telecommand packet to telecommand packet processor task 
        // via message que:
        ret_val = rt_queue_write(&telecmd_msg_queue,&buf,\
            telecmd_pkt_size,Q_NORMAL); // Append message to queue

        // Check success:
        if (ret_val > 0) {
            // Print:
            rt_printf("%d (RX_TELECMD_PKT_TASK)"
                    " Telecommand packet sent to"
                    " processor task\n",time(NULL));
        } else {
            // NEED ERROR HANDLING
        }

        // Reset counters:
        bytes_left = telecmd_pkt_size; // Bytes left to read
        bytes_read = 0;                // Bytes read
    }

    // This will never be reached:
    return;
}
