///////////////////////////////////////////////////////////////////////////////
//
// Transmit Telemetry Packet
//
// Task responsible for transmitting telemetry packets to the downlink
// serial port. Telemetry packet transfer frames are received via message
// queue from filter table or retrieve file task. Telemetry packet transfer
// frames are fixed length and consist of
//     - APID (origin)
//     - Telemetry Packet
//
// The telemetry packet is removed from the transfer frame for downlinking.
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
// Created: February 3, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdlib.h>  // Standard library
#include <unistd.h>  // UNIX standard function definitions
#include <errno.h>   // Error number definitions.
#include <string.h>  // String function definitions 
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types

// Xenomai libraries:
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <open_port.h>  // Open serial port function declaration
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations

// Macro definitions:
#define TLM_PKT_XFR_FRM_SIZE 1089 // Telemetry packet transfer frame size in bytes
#define TLM_PKT_SIZE         1080 // Telemetry packet in bytes
#define B1000000          0010010 // Baud rate (as defined in terminos.h)

// Message queue definitions:
RT_QUEUE tx_tlm_pkt_msg_queue;  // For telemetry packets
                                // (flt_tbl_task/rtrv_file_task
                                // --> tx_tlm_pkt_task)

// Semaphore definitions:
RT_SEM tx_tlm_pkt_sem; // For tx_tlm_pkt_task, flt_tbl_task, and rtrv_file_task
                       // synchronization

void tx_tlm_pkt(void* arg) {
    // Print:
    rt_printf("%d (TX_TLM_PKT) Task started\n",time(NULL));

    // Definitions and initializations:
    uint8_t i;
    int16_t ret_val; // Function return value
    int8_t  fd;      // File descriptor for port

    char* port = "/dev/pts/4"; // Downlink

    char tlm_pkt_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                            // packet transfer frame
                                            // buffer
    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer
    // Open port:
    fd = open_port(port,B1000000);

    // Check for success:
    if (fd > 0) {
        // Success:
        rt_printf("%d (TX_TLM_PKT) Downlink serial port opened and"
            " configured\n",time(NULL));
    } else {
        // Print:
        rt_printf("%d (TX_TLM_PKT) Error opening and configuring downlink"
            " serial port\n",time(NULL));
        // NEED ERROR HANDLING
    }

    // Task synchronize with filter table and retrieve file task
    // (tell task that it is now ready to receive transfer frames)
    rt_printf("%d (TX_TLM_PKT) Ready to receive telemetry packet transfer"
        " frames\n",time(NULL));
    
    // Signal:
    for (i = 0; i < 2; ++i)
        rt_sem_v(&tx_tlm_pkt_sem);

    // Infinite loop to receive telemetry packet transfer frames via message
    // queue and direct to either transmit telemetry packet task or create file
    // task:
    while (1) {
        // Read transfer frame from message queue:
        ret_val = rt_queue_read(&tx_tlm_pkt_msg_queue,\
            &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,TM_INFINITE);

        // Check success:
        if (ret_val == TLM_PKT_XFR_FRM_SIZE) {
            // Print
            rt_printf("%d (TX_TLM_PKT) Received telemetry packet transfer"
                " frame\n",time(NULL));
        // Error:
        } else {
            // Print
            rt_printf("%d (TX_TLM_PKT) Error receiving telemetry packet"
                " transfer frame\n",time(NULL));
            // NEED ERROR HANDLING                                          
        }

        // Remove telemetry packet from transfer frame:
        memcpy(&tlm_pkt_buf,tlm_pkt_xfr_frm_buf+9,TLM_PKT_SIZE);

        // Write telemetry packet to downlink serial port:
        ret_val = write(fd,&tlm_pkt_buf,TLM_PKT_SIZE);

        // Check success:
        if (ret_val == TLM_PKT_SIZE) {
            // Print
            rt_printf("%d (TX_TLM_PKT) Transmitted telemetry packet\n",\
                time(NULL));
        // Error:
        } else {
            // Print
            rt_printf("%d (TX_TLM_PKT) Error transmitting telemetry packet\n",\
                time(NULL));
            // NEED ERROR HANDLING                                          
        }
    }
}