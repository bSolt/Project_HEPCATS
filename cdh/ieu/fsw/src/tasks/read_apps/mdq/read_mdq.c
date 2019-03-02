///////////////////////////////////////////////////////////////////////////////
//
// Read MDQ (Magnetometer DAQ)
//
// Task responsible for reading data from the magnetometer DAQ, creating
// telemetry packet transfer frames from the source data, and sending those
// frames to the filter table task via message queue for for either downlink
// or recording to storage for later downlink.
//
// DAQ initialization is performed by the command mdq task by calling a
// function that enables channels 0, 1, and 2 and sets the sampling rate to
// 240 Hz for each channel (among other things). The command mdq task also
// starts and stops the DAQ from scanning (collecting data) which either
// allows or denies this task to read from the DAQ.
//
// The DAQ sends a transmission burst of size defined by the packet size for
// each channel. The packet size is set to 256 bytes by default and is what is
// expected in this task. A transmission is only made when the packet is full
// of data.
//
// This task requests a bulk transfer request from the device reading in one
// packet of data at 256 bytes from each channel (0, 1, and 2) only when the
// DAQ is readable (i.e. scanning). The device returns a 16 bit value for each
// channel. Libusb converts this whole message into a char* array. Each char is
// 8 bits, so the measurement in each channel is composed of two chars: the one
// at 2n and the one at 2n+1.
//
// White paper for communication protocol:
// https://www.dataq.com/resources/pdfs/misc/DI-4108-DI-4208-Protocol.pdf
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

// Xenomai libraries:
#include <alchemy/task.h>  // Task management service
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// External libraries:
#include <libusb-1.0/libusb.h> // Library to access USB devices

// Header files:
#include <msg_queues.h>          // Message queue variable declarations
#include <sems.h>                // Semaphore variable declarations
#include <mdq_dev.h>             // Magnetometer DAQ device variable
                                 // declarations
#include <crt_tlm_pkt_xfr_frm.h> // Create telemetry packet transfer frame
                                 // function declaration

// Macro definitions:
#define DAQ_PKT_SIZE      768     // Magnetometer DAQ packet size in bytes.
#define DAQ_SRC_DAT_SIZE 1064     // Magnetometer DAQ source data message queue
                                  // size in bytes
#define TLM_PKT_XFR_FRM_SIZE 1089 // Telemetry transfer frame size in bytes

#define APID_MDQ 0xC8 // Magnetometer DAQ origin

// Message queue definitions:
RT_QUEUE flt_tbl_msg_queue; // For telemetry packet transfer frames
                            // (read_mdq/img/get_hk_tlm --> flt_tbl_task)

// Semaphore definitions:
RT_SEM flt_tbl_sem;  // For flt_tbl_task, read_mdq/img, and get_hk_tlm task
                     // synchronization
RT_SEM mdq_init_sem; // For init_mdq and read_mdq task synchronization
RT_SEM read_mdq_sem; // For cmd_mdq and read_mdq task synchronization to
                     // indicate when DAQ is readable (scanning)

// Variable declarations:
libusb_device_handle* dev_hdl;    // Device handle
libusb_context* ctx;              // libusb session
uint16_t tlm_pkt_xfr_frm_seq_cnt; // Packet sequence count

void read_mdq(void) {
    // Print:
    rt_printf("%d (READ_MDQ_TASK) Task started\n",time(NULL));

    // Task synchronize with filter table task:
    // (wait for task to be ready to telemetry packet transfer frames)
    rt_printf("%d (READ_MDQ_TASK) Waiting for filter table task"
        " to be ready\n",time(NULL));

    // Wait for signals:
    rt_sem_p(&flt_tbl_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (READ_MDQ_TASK) Filter table task is ready;"
        " continuing\n",time(NULL));

    // Synchronization with initialize magnetometer DAQ called by command
    // DAQ task:
    // (Wait for initialization)
    rt_printf("%d (READ_MDQ_TASK) Waiting for magnetometer DAQ initialization"
        " to complete\n",time(NULL));

    // Wait for signal:
    rt_sem_p(&mdq_init_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (READ_MDQ_TASK) DAQ initialization complete;"
        " continuing\n",time(NULL));

    // Definitions and initializations:
    int8_t ret_val; // Function return value

    uint16_t bytes = 0; // Bytes written or received

    uint8_t tlm_pkt_xfr_frm_grp_flg = 3; // Packet sequence grouping flag

    char mdq_buf[DAQ_PKT_SIZE]; // Buffer for DAQ

    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer

    // Print:
    rt_printf("%d (READ_MDQ_TASK) Ready to read magnetometer DAQ and"
        " create telemetry packets\n",time(NULL));

    // Infinite loop to read magnetometer DAQ data if the DAQ is currently
    // readable (currently scanning). This is done using a semaphore that is
    // incremented and decremented by the command mdq task. Libusb is used to
    // read data from the USB via bulk transfer request.
    while (1) {
        // Decrease semaphore (if unable, wait) then immediately increase it:
        // (If unable to decrease, the semaphore will be be incremented by
        // command mdq once the start scanning command is received. The
        // immediate increase allows the next loop to immediately
        // continue unless command mdq decreased the semaphore in the meantime
        // after receiving the "stop" scanning command)
        rt_sem_p(&read_mdq_sem,TM_INFINITE); rt_sem_v(&read_mdq_sem);

        // Get data from the DAQ:
        ret_val = libusb_bulk_transfer(dev_hdl,(1|LIBUSB_ENDPOINT_IN),\
            (unsigned char*) mdq_buf,DAQ_PKT_SIZE,&bytes,0);

        // (If no return error and number of bytes written is expected)
        if ((ret_val == 0) && (bytes == DAQ_PKT_SIZE)) {
            // Increment sequence count:
            tlm_pkt_xfr_frm_seq_cnt++;

            // Force counter roll over at 16384:
            // (the field in the packet that sequence occupies is only
            // 14 bits)
            if (tlm_pkt_xfr_frm_seq_cnt > 16383) {
                tlm_pkt_xfr_frm_seq_cnt = 1; // 1 because it's logical
                                             // (0 ain't)
            } 

            // Create transfer frame:
            crt_tlm_pkt_xfr_frm(mdq_buf,DAQ_PKT_SIZE,\
                tlm_pkt_xfr_frm_buf,APID_MDQ,\
                tlm_pkt_xfr_frm_grp_flg,tlm_pkt_xfr_frm_seq_cnt);

            // Send transfer frame to filter table task via message queue:
            ret_val = rt_queue_write(&flt_tbl_msg_queue,\
                &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                Q_NORMAL); // Append message to queue

            // Check success:
            if ((ret_val > 0) || (ret_val == 0)) {
                // Print:
                rt_printf("%d (READ_IMG_TASK) Telemetry packet transfer"
                    " frame sent to filter table task\n",time(NULL));
            } else if (ret_val == -ENOMEM) {
                // Wait for a set time to allow filter table task to
                // process message queue:
                sleep(0.35);

                // Send transfer frame to filter table task via message queue:
                ret_val = rt_queue_write(&flt_tbl_msg_queue,\
                    &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                    Q_NORMAL); // Append message to queue
            } else {
                // Print:
                rt_printf("%d (READ_IMG_TASK) Error sending telemetry"
                    " packet transfer frame\n",time(NULL));
                // NEED ERROR HANDLING
            }
        } else if (bytes != DAQ_PKT_SIZE) {
            // Print:
            rt_printf("%d (READ_MDQ_TASK) Number of bytes read (%d) from DAQ"
                " is not what is expected; ignoring data\n",time(NULL),bytes);
        } else {
            // Print:
            rt_printf("%d (SEND_MDQ_CMD) Error reading DAQ;"
                " error %d\n",time(NULL),ret_val);
        }
    }

    // Will never reach this
    return;
}