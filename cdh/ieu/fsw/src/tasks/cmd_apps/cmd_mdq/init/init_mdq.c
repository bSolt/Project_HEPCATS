///////////////////////////////////////////////////////////////////////////////
//
// Initialize Magnetometer DAQ
//
// Task responsible for initializing the DAQ with libusb-1.0. The device
// handle is defined here and used when requesting bulk data transfers or
// sending messages (commands) to the DAQ.
//
// From the manufacturer, must use the following constants:
//     PID = 0x2108
//     VID = 0x0683
// (https://www.dataq.com/resources/pdfs/misc/DI-4108-DI-4208-Protocol.pdf)
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
// - ret_val
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: February 24, 2019
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
#include <alchemy/sem.h>   // Semaphore services

// External libraries:
#include <libusb-1.0/libusb.h> // Library to access USB devices

// Header files:
#include <mdq_dev.h>      // Magnetometer DAQ device variable declarations
#include <send_mdq_cmd.h> // Send Magnetometer DAQ command function declaration

// Macro definitions:
#define PID 0x4108 // DAQ product I.D.
#define VID 0x0683 // DAQ vendor I.D.

// Semaphore definitions:
RT_SEM mdq_init_sem; // For init_mdq and read_mdq task synchronization

// Variable definitions:
libusb_device_handle* dev_hdl; // Device handle
libusb_context* ctx;           // libusb session

int8_t init_mdq() {
    // Definitions and initializations:
    int8_t ret_val = 0; // Function return value

    libusb_device** dev_list; // Used to retrieve a list of devices

    // Initialize libusb:
    ret_val = libusb_init(&ctx); // Initialize library session

    // Check success:
    if (ret_val < 0) {
        // Print:
        rt_printf("%d (INIT_MDQ) Could not initialize library"
            " session; aborting with error %d\n",time(NULL),ret_val);

        // Exit
        return ret_val;
    }

    // Warnings:
    libusb_set_debug(ctx,3); // Set verbosity level to 3

    // Get the list of devices:
    ret_val = libusb_get_device_list(ctx,&dev_list);

    // Check success:
    if (ret_val < 0) {
        // Print:
        rt_printf("%d (INIT_MDQ) Could not get device list; aborting"
            " with error %d\n",time(NULL),ret_val);

        // Exit
        return ret_val;
    }

    // Get DAQ device handle by opening device:
    dev_hdl = libusb_open_device_with_vid_pid(ctx,VID,PID);

    // Check success:
    if (dev_hdl == NULL) {
        // Print:
        rt_printf("%d (INIT_MDQ) Could not open device; aborting\n",\
            time(NULL));

        return -1;
    }

    // Free device list:
    // (Unreference devices in it)
    libusb_free_device_list(dev_list,1);

    // Check if kernel driver is active:
    ret_val = libusb_kernel_driver_active(dev_hdl,0);

    // If attached, then detach it:
    if (ret_val == 1) {
        // Print:
        rt_printf("%d (INIT_MDQ) Kernel driver is active\n",time(NULL));

        // Detach:
        ret_val = libusb_detach_kernel_driver(dev_hdl,0);

        // Check:
        if (ret_val == 0) {
            // Print:
            rt_printf("%d (INIT_MDQ) Kernel driver detached\n",time(NULL));
        }
    }

    // Claim interface:
    ret_val = libusb_claim_interface(dev_hdl,0);

    // Check success:
    if (ret_val < 0) {
        // Print:
        rt_printf("%d (INIT_MDQ) Could not claim interface; aborting with"
            " error %d\n",time(NULL),ret_val);

        return ret_val;
    }

    // Set LED to yellow to indicate that initialization is in progress:
    ret_val = send_mdq_cmd("led 6");

    // Request packet size of 256 bytes:
    // (This packet size is returned from each channel meaning that each
    // transmission from the DAQ will be 768 bytes.)
    ret_val = send_mdq_cmd("ps 4");

    // Active channels 0, 1, and 2 with range +\-10V and place them in the
    // scanning list in that order:
    // (The slist command has two arguments: offset and config. The offset
    // defines the index in which the channel is placed in the scan list. The
    // config is a 16 bit number that defines the channel to activate and its
    // range. How this 16 bit number is determined is found in the white
    // paper.)
    ret_val = send_mdq_cmd("slist 0 0");
    ret_val = send_mdq_cmd("slist 1 1");
    ret_val = send_mdq_cmd("slist 2 2");

    // Set decimation value:
    // (Decimation value of 1 with CIC mode means that one sample is used
    // for calculation. Four stages of 2-sample moving window averages are
    // applied and all filtered values of returned)
    ret_val = send_mdq_cmd("dec 100");

    // Define scan rate such that sample rate per channel is 240 Hz for
    // for this decimation value:
    ret_val = send_mdq_cmd("srate 2500");

    // Define acquisition mode to be CIC (cascaded integrator–comb filter):
    ret_val = send_mdq_cmd("filter * 1");

    // Print:
    rt_printf("%d (INIT_MDQ) DAQ initialization complete\n",time(NULL));

    // Signal initialization has been complete:
    rt_sem_v(&mdq_init_sem);

    // Return:
    return ret_val;
}
