///////////////////////////////////////////////////////////////////////////////
//
// Read USB (Magnetometer) DAQ
//
// Task responsible for reading data from the magnetometer DAQ, creating
// telemetry packet transfer frames from the source data, and sending those
// frames to the filter table task via message queue for for either downlink
// or recording to storage for later downlink.
//
// Libusb is used to control data transfers from the DAQ. From the
// manufacturer, must use the following constants:
//     PID = 0x2108
//     VID = 0x0683
// The DAQ sends a transmission burst of size defined by the packet size.
// The packet size is set to #### bytes by default and is what is expected
// in this task. A transmission is only made when the packet is full of data
// (https://www.dataq.com/resources/pdfs/misc/di-2108-usb-daq-protocol.pdf)
//
// This task requests a bulk transfer request from the device reading in one
// packet of data. The device returns a 16 bit value for each channel. Libusb
// converts this whole message into a char* array. Each char is 8 bits, so
// the measurement in each channel is composed of two chars: the one at 2n and
// the one at 2n+1. Conversion from char to a 16 bit number takes place before
// telemetry packet transfer frames are created.
// (https://github.com/samdejong86/DI-2108-P-High-Speed-DAQ/blob/master/DI-2108-P.cpp)
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - Xenomai 3 / Cobalt
// - libusb-1.0
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

// External libraries:
#include <libusb-1.0/libusb.h> // Library to access USB devices

// Xenomai libraries:
#include <alchemy/task.h>  // Task management service
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations

// Macro definitions:
#define DAQ_PKT_SIZE      128 // Magnetometer DAQ packet size in bytes.
#define DAQ_SRC_DAT_SIZE 1064 // Magnetometer DAQ source data message queue
                              // size in bytes

// Message queue definitions:
RT_QUEUE flt_tbl_msg_queue; // For telemetry packet transfer frames
                            // (read_usb/get_hk_tlm --> flt_tbl_task)

// Semaphore definitions:
RT_SEM flt_tbl_sem; // For flt_tbl_task, read_usb, and get_hk_tlm task
                    // synchronization

void read_usb_daq(void) {
    // Print:
    rt_printf("%d (READ_USB_DAQ_TASK) Task started\n",time(NULL));

    // Task synchronize with filter table task:
    // (wait for task to be ready to telemetry packet transfer frames)
    rt_printf("%d (READ_USB_DAQ_TASK) Waiting for filter table task"
        " to be ready\n",time(NULL));

    // Wait for signals:
    rt_sem_p(&flt_tbl_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (READ_USB_DAQ_TASK) Filter table task is ready;"
        " continuing\n",time(NULL));

    // Definitions and initializations:
    int8_t ret_val; // Function return value

    char usb_buf[DAQ_PKT_SIZE];             // Buffer for USB read
    char daq_src_dat_buf[DAQ_SRC_DAT_SIZE]; // Buffer for DAQ source data

    libusb_device** devs;       // Used to retrieve a list of devices
    libusb_context* ctx = NULL; // libusb session

    // Initialize libusb:
    ret_val = libusb_init(&ctx); // Initialize library session

    // Check success:
    if (ret_val == 0) {
        // Print:
        rt_printf("%d (READ_USB_DAQ_TASK) Libusb initialization successful" 
            "; continuing \n",time(NULL));
    } else {
        // Print:
        rt_printf("%d (READ_USB_DAQ_TASK) Libusb initialization error" 
            " \n",time(NULL));
        // NEED ERROR HANDLING
    }

    // Warnings:
    libusb_set_debug(ctx,3); // Set verbosity level to 3

    // Print:
    rt_printf("%d (READ_USB_DAQ_TASK) Ready to read magnetometer DAQ and"
        " create telemetry packets\n",time(NULL));

    // Add device setup and all the jazz ...

    // Infinite loop to read magnetometer DAQ data. Libusb is used to read data
    // from the USB via bulk transfer request who's size is
    while (1) {
        // Read DAQ packet from USB using bulk transfer request until packets
        // received is > mod(DAQ_SRC_DAT_SIZE,DAQ_PKT_SIZE). Copy USB buffer to
        // message queue buffer during each iteration to fill message. When
        // exiting the while loop, will have the message buffer completely
        // filled.
        sleep(1000);

        // PLACEHOLDER
    }

    return;
}