///////////////////////////////////////////////////////////////////////////////
//
// Create Telemetry Packet
//
// Task responsible for creating telemetry packet transfer frames from source
// data via two message queue. Source data originates from USB read tasks
// that read data from the magnetometer DAQ and imaging. Separate message
// queues are utilized by each USB read task as source data size is not
// consistent between the DAQ and camera.
//
// Once source data is read from the queue, telemetry packet transfer frame
// are created by calling the create telemetry packet transfer frame function
// given the message buffer and origin. Telemetry packet transfer frames are
// fixed length and consist of
//     - tlm_pkt_xfr_frm_apid (origin)
//     - Telemetry Packet
//
// Telemetry packet transfer frames are then sent to the filter table task
// via message queue for either immediate downlink or storage on secondary
// memory.
//
// Note, images are split across many telemetry packets. Create telemetry
// packet transfer frame function is called mod(DAQ_SRC_DAT_SIZE,1064) times.
// Each transfer frame is sent individually to the filter table task, but the
// entire image is sent in one go (i.e. will not process new source data until
// the image is sent).
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
#include <string.h>  // String function definitions
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types

// Xenomai libraries:
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <msg_queues.h>          // Message queue variable declarations
#include <sems.h>                // Semaphore variable declarations
#include <crt_tlm_pkt_xfr_frm.h> // Create telemetry packet transfer frame
                                 // function declaration

// Macro definitions:
#define TLM_PKT_XFR_FRM_SIZE 1082 // Telemetry transfer frame size in bytes
#define TLM_PKT_USR_DAT_SIZE 1064 // Telemetry packet user data size in bytes
#define DAQ_SRC_DAT_SIZE     1064 // Magnetometer DAQ source data message queue
                                  // size in bytes
#define IMG_SRC_DAT_SIZE     3000 // Imaging source data message queue
                                  // size in bytes (UPDATE)

#define APID_IMG 0x64 // Image destination tlm_pkt_xfr_frm_apid
#define APID_MDQ 0xC8 // Magnetometer DAQ destination tlm_pkt_xfr_frm_apid

// Message queue definitions:
RT_QUEUE daq_src_dat_msg_queue; // For magnetometer DAQ source data
                                // (read_usb_daq_task --> crt_tlm_pkt_task)
RT_QUEUE img_src_dat_msg_queue; // For imaging source data
                                // (read_usb_im_task --> crt_tlm_pkt_task)
RT_QUEUE flt_tbl_msg_queue;     // For telemetry packet transfer frames
                                // (crt_tlm_pkt_task --> flt_tbl_task)
// Semaphore definitions:
RT_SEM flt_tbl_sem; // For flt_tbl_task and crt_tlm_pkt_task synchronization

void crt_tlm_pkt(void) {
    // Print:
    rt_printf("%d (CRT_TLM_PKT_TASK) Task started\n",time(NULL));

    // Task synchronize with filter table task:
    // (wait for task to be ready to receive transfer frames)
    rt_printf("%d (CRT_TLM_PKT_TASK) Waiting for filter table task to be"
        " ready \n",time(NULL));

    // Wait for signal:
    rt_sem_p(&flt_tbl_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (CRT_TLM_PKT_TASK) Filter table task is ready;"
        " continuing \n",time(NULL));
    
    // Definitions and initializations:
    uint8_t  i;
    int16_t  ret_val = 0;          // Function return value
    uint16_t tlm_pkt_xfr_frm_apid; // Source data origin
    uint8_t  grp_flg;              // Packet sequence grouping flag
    uint16_t copy_size;            // Buffer copy size

    uint16_t num_tlm_pkt_xfr_frm_req; // Required number of tranfer frames

    uint8_t rx_src_dat_flg   = 0; // Received source data flag
    uint8_t skip_daq_src_flg = 0; // Skip checking magnetometer DAQ source
                                  // data availability

    char tmp_buffer[TLM_PKT_USR_DAT_SIZE];  // Temporary buffer
    char daq_src_dat_buf[DAQ_SRC_DAT_SIZE]; // Buffer for DAQ source data
    char img_src_dat_buf[IMG_SRC_DAT_SIZE]; // Buffer for image source data
    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer

    RTIME timeout = 5e9; // Read message queue timeout in nanoseconds

    // Task synchronize with read USB tasks:
    // (tell tasks that it is now ready to receive source data)
    rt_printf("%d (CRT_TLM_PKT_TASK) Ready to receive source data"
        "\n",time(NULL));

    // Signal:
    for (i = 0; i < 2; ++i)
        rt_sem_v(&crt_tlm_pkt_sem);
    
    // Infinite loop to receive source data from read USB tasks via two message
    // queues. Each message queue has a time limit to wait for a message
    // (source data) to be available; moreover, if the timeout expires on one
    // message queue, then the other message queue is read. This continues
    // until a message is available to be read on one of the queues. Once
    // source data is received, telemetry packet transfer frames
    // are created and sent to filter table task via message queue for
    // either immediate downlink or storage on secondary memory.
    while (1) {
        while (rx_src_dat_flg == 0) {
            // Check if magnetometer DAQ source data was just received. If yes,
            // skip DAQ source data availability check and check imaging. If
            // no, check DAQ source data availability:
            // (This check prevents imaging source data from being ignored)
            if (skip_daq_src_flg == 0) {
                // Read magnetometer DAQ source data from message queue
                // (if it is there):
                ret_val = rt_queue_read(&daq_src_dat_msg_queue,\
                    &daq_src_dat_buf,DAQ_SRC_DAT_SIZE,timeout);

                // Check success:
                if (ret_val == DAQ_SRC_DAT_SIZE) {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Received magnetometer DAQ"
                        " source data \n",time(NULL));

                    // Set tlm_pkt_xfr_frm_apid:
                    tlm_pkt_xfr_frm_apid = APID_MDQ;

                    // Set flags:
                    rx_src_dat_flg = 1;   // Received source data flag:
                    skip_daq_src_flg = 1; // Skip checking magnetometer DAQ
                                          // source data availability next loop 
                // timeout:
                } else if (ret_val == -ETIMEDOUT) {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Timed out waiting for"
                        " magnetometer DAQ source data; checking imaging\n",\
                        time(NULL));
                // Error:
                } else {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Error receiving"
                        " magnetometer DAQ source data\n",time(NULL));
                    // NEED ERROR HANDLING                                          
                }
            }

            // Check if magnetometer source data was just received. If yes,
            // skip imaging source data availability check and create telemetry
            // packet. If no, check imaging source data availability:
            // (This check allows magnetometer DAQ source data telemetry
            // packets to be created now)
            if (rx_src_dat_flg == 0) {
                // Read imaging source data from message queue (if it's there):
                ret_val = rt_queue_read(&img_src_dat_msg_queue,\
                    &img_src_dat_buf,IMG_SRC_DAT_SIZE,timeout);

                // Check success:
                if (ret_val == IMG_SRC_DAT_SIZE) {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Received imaging source" 
                        " data \n",time(NULL));

                    // Set tlm_pkt_xfr_frm_apid:
                    tlm_pkt_xfr_frm_apid = APID_IMG;

                    // Set flags:
                    rx_src_dat_flg = 1;   // Received source data
                    skip_daq_src_flg = 0; // Don't skip checking magnetometer DAQ
                                          // source data availability next loop 
                // timeout:
                } else if (ret_val == -ETIMEDOUT) {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Timed out waiting for"
                        " imaging source data; checking magnetometer DAQ\n",\
                        time(NULL));

                    // Set flags:
                    skip_daq_src_flg = 0; // Don't skip checking magnetometer DAQ
                                          // source data availability next loop 
                // Error:
                } else {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Error receiving imaging"
                        " source data\n",time(NULL));
                    // NEED ERROR HANDLING
                }
            }
        }

        // Print:
        rt_printf("%d (CRT_TLM_PKT_TASK) Creating telemetry packet transfer"
        " frame(s)\n",time(NULL));

        // Create transfer frames depending on which source data was just
        // received:
        if (tlm_pkt_xfr_frm_apid == APID_MDQ) {
            // Set grouping flag:
            grp_flg = 3; // Unsegmented data

            // Create transfer frame:
            crt_tlm_pkt_xfr_frm(daq_src_dat_buf,DAQ_SRC_DAT_SIZE,\
                tlm_pkt_xfr_frm_buf,tlm_pkt_xfr_frm_apid,grp_flg);

            // Send transfer frame to filter table task via message queue:
            ret_val = rt_queue_write(&flt_tbl_msg_queue,&tlm_pkt_xfr_frm_buf,\
                TLM_PKT_XFR_FRM_SIZE,Q_NORMAL); // Append message to queue

            // Check success:
            if ((ret_val > 0) || (ret_val == 0)) {
                // Print:
                rt_printf("%d (CRT_TLM_PKT_TASK) Telemetry packet transfer"
                    " frame sent to filter table task\n",time(NULL));
            } else {
                // Print:
                rt_printf("%d (CRT_TLM_PKT_TASK) Error sending telemetry"
                    " packet transfer frame\n",time(NULL));
                // NEED ERROR HANDLING
            } 
        } else if (tlm_pkt_xfr_frm_apid == APID_IMG) {
            // Determine how many transfer frames are required:
            num_tlm_pkt_xfr_frm_req = \
                (IMG_SRC_DAT_SIZE/TLM_PKT_USR_DAT_SIZE) + 1; 

            // Loop to create required number of transfer frames:
            for (i = 0; i < num_tlm_pkt_xfr_frm_req; ++i) {
                // Set grouping flag based off iteration:
                if (i == 0) {
                    // Set grouping flag:
                    grp_flg = 1; // First segment

                    // Set copy size in bytes:
                    copy_size = TLM_PKT_USR_DAT_SIZE;
                } else if (i == (num_tlm_pkt_xfr_frm_req - 1)) {
                    // Set grouping flag:
                    grp_flg = 2; // Last segment

                    // Set copy size in bytes:
                    copy_size = IMG_SRC_DAT_SIZE - TLM_PKT_USR_DAT_SIZE*i;
                } else {
                    // Set grouping flag:
                    grp_flg = 0; // Continuation segment

                    // Set copy size in bytes:
                    copy_size = TLM_PKT_USR_DAT_SIZE;
                }

                // Copy source data buffer to temporary buffer using dynamic
                // offset and size:
                memcpy(tmp_buffer,img_src_dat_buf+TLM_PKT_USR_DAT_SIZE*i,\
                    copy_size);

                // Create transfer frame:
                crt_tlm_pkt_xfr_frm(tmp_buffer,copy_size,\
                    tlm_pkt_xfr_frm_buf,tlm_pkt_xfr_frm_apid,grp_flg);

                // Send transfer frame to filter table task via message queue:
                ret_val = rt_queue_write(&flt_tbl_msg_queue,&tlm_pkt_xfr_frm_buf,\
                    TLM_PKT_XFR_FRM_SIZE,Q_NORMAL); // Append message to queue

                // Check success:
                if ((ret_val > 0) || (ret_val == 0)) {
                    // Print:
                    //rt_printf("%d (CRT_TLM_PKT_TASK) Telemetry packet transfer"
                    //    " frame sent to filter table task\n",time(NULL));
                } else {
                    // Print:
                    rt_printf("%d (CRT_TLM_PKT_TASK) Error sending telemetry"
                        " packet transfer frame\n",time(NULL));
                    // NEED ERROR HANDLING
                }
            }

            // Print:
            rt_printf("%d (CRT_TLM_PKT_TASK) All %d telemetry packet transfer"
                    " frame sent to filter table task\n",time(NULL),\
                    num_tlm_pkt_xfr_frm_req);
        }

        // Reset receive source data flag:
        rx_src_dat_flg = 0;
    }

    // Will never this reach this
    return;
}