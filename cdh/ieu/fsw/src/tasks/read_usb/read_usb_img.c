///////////////////////////////////////////////////////////////////////////////
//
// Read USB Imaging
//
// Task responsible for reading data from imaging, interfacing with image
// processing software, and passing data to create telemetry packet task.
//
// AND...
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
// Created: January 28, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdlib.h>  // Standard library
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

// Macro definitions:
#define IMG_SRC_DAT_SIZE  3000 // Imaging source data message queue
                               // size in bytes (UPDATE)

// Message queue definitions:
RT_QUEUE img_src_dat_msg_queue; // For imaging source data
                                // (read_usb_im_task --> crt_tlm_pkt_task)

void read_usb_img(void) {
    // Print:
    rt_printf("%d (READ_USB_IMG_TASK) Task started\n",time(NULL));

    // Task synchronize with create telemetry packet:
    // (wait for task to be ready to receive source data)
    rt_printf("%d (READ_USB_IMG_TASK) Waiting for create telemetry packet task"
        " to be ready\n",time(NULL));

    // Wait for signals:
    rt_sem_p(&crt_tlm_pkt_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (READ_USB_IMG_TASK) Create telemetry packet task is" 
        " ready; continuing\n",time(NULL));

    int8_t ret_val;
    char daq_src_dat_buf[IMG_SRC_DAT_SIZE]; // Buffer for source data

     while (1) {
        sleep(60);

        // Send source data to create telemetry packet task via message queue:
        ret_val = rt_queue_write(&img_src_dat_msg_queue,&daq_src_dat_buf,\
            IMG_SRC_DAT_SIZE,Q_NORMAL);

        // Check success:
        if ((ret_val > 0) || (ret_val == 0)) {
            // Print:
            rt_printf("%d (READ_USB_IMG_TASK) Source data"
                " sent to create telemetry packet task\n",time(NULL));
        } else {
            rt_printf("%d (READ_USB_IMG_TASK) Error sending"
                " source data to create telemetry packet task\n",\
                time(NULL));
            // NEED ERROR HANDLING
        }

        // PLACEHOLDER
    }

    return;
}