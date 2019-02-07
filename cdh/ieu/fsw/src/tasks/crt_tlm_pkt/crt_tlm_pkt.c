///////////////////////////////////////////////////////////////////////////////
//
// Create Telemetry Packet
//
// Task responsible for creating telemetry packet transfer frames from source
// data and housekeeping telemetry via three message queue. Source data
// originates from USB read tasks that read data from the magnetometer DAQ and
// imaging. Housekeeping telemetry originates for get hk telemetry task.
// Separate message queues are utilized by each USB read task and get
// housekeeping task as message size is not consistent between the three.
//
// Once source data or housekeeping telemetry is read from the queue, telemetry
// packet transfer frame are created by calling the create telemetry packet
// transfer frame function given the message buffer and other packet inputs.
// Telemetry packet transfer frames are fixed length and consist of
//     - Packet Identification
//       - APID (origin)
//       - Group flag (packet sequence)
//     - Creation Time
//       - Second
//       - Millisecond
//     - Telemetry Packet
//
// Telemetry packet transfer frames, once created, are then sent individually
// to the filter table task via message queue for either immediate downlink or
// storage on secondary memory.
//
// Note, images are split across many telemetry packets due to the size of the
// image; thus, create telemetry packet transfer frame function is called 
// (IMG_SRC_DAT_SIZE/TLM_PKT_USR_DAT_SIZE + 1) times with the last call
// passing partially filed source data to the function. The remained of the
// space is filled with NULL during packet creation (this is also true for
// housekeeping telemetry).
//
// Each transfer frame, once created, is sent individually to the filter
// table task to be directed for either downlink or storage.
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
#define TLM_PKT_XFR_FRM_SIZE 1089 // Telemetry transfer frame size in bytes
#define TLM_PKT_USR_DAT_SIZE 1064 // Telemetry packet user data size in bytes
#define DAQ_SRC_DAT_SIZE     1064 // Magnetometer DAQ source data message queue
                                  // size in bytes
#define IMG_SRC_DAT_SIZE     3000 // Imaging source data message queue
                                  // size in bytes (UPDATE)
#define HK_TLM_SIZE             7 // Housekeeping telemetry message queue size
                                  // in bytes

#define APID_SW  0x00 // Software origin
#define APID_IMG 0x64 // Image origin
#define APID_MDQ 0xC8 // Magnetometer DAQ origin

// Message queue definitions:
RT_QUEUE daq_src_dat_msg_queue; // For magnetometer DAQ source data
                                // (read_usb_daq_task --> crt_tlm_pkt_task)
RT_QUEUE img_src_dat_msg_queue; // For imaging source data
                                // (read_usb_im_task --> crt_tlm_pkt_task)
RT_QUEUE flt_tbl_msg_queue;     // For telemetry packet transfer frames
                                // (crt_tlm_pkt_task --> flt_tbl_task)
RT_QUEUE hk_tlm_msg_queue;      // For housekeeping telemetry
                                // (get_hk_tlm_task --> crt_tlm_pkt_task)

// Semaphore definitions:
RT_SEM flt_tbl_sem;     // For flt_tbl_task and crt_tlm_pkt_task
                        // synchronization
RT_SEM crt_tlm_pkt_sem; // For crt_tlm_pkt_task and read_usb tasks
                        // synchronization

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
    uint8_t i;

    int16_t  ret_val;   // Function return value
    uint16_t copy_size; // Buffer copy size

    uint16_t tlm_pkt_xfr_frm_apid;        // Source data origin
    uint8_t  tlm_pkt_xfr_frm_grp_flg;     // Packet sequence grouping flag
    uint16_t tlm_pkt_xfr_frm_seq_cnt = 0; // Packet sequence count

    uint16_t num_tlm_pkt_xfr_frm_req; // Required number of tranfer frames

    uint8_t rx_msg_flg = 0;       // Received message flag
    uint8_t skip_hk_tlm_flg = 0;  // Skip checking housekeeping telemetry
                                  // message queue
    uint8_t skip_daq_src_flg = 0; // Skip checking magnetometer DAQ source
                                  // message queue

    char tmp_buffer[TLM_PKT_USR_DAT_SIZE];  // Temporary buffer
    char daq_src_dat_buf[DAQ_SRC_DAT_SIZE]; // Buffer for DAQ source data
    char img_src_dat_buf[IMG_SRC_DAT_SIZE]; // Buffer for image source data
    char hk_tlm_buf[HK_TLM_SIZE];           // Buffer for image source data

    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer

    RTIME queue_timeout = 0.5e9; // Read message queue timeout in nanoseconds

    // Task synchronize with read USB tasks:
    // (tell tasks that it is now ready to receive source data)
    rt_printf("%d (CRT_TLM_PKT_TASK) Ready to receive source data"
        "\n",time(NULL));

    // Signal:
    for (i = 0; i < 3; ++i) {
        rt_sem_v(&crt_tlm_pkt_sem);
    }
    
    // Infinite loop to receive source data from read USB tasks or housekeeping
    // telemetry via three message queues. Each message queue has a time limit
    // to wait for a message to be received; moreover, if the timeout expires
    // on one message queue, then the other message queues are read. This
    // continues until a message is available to be read on one of the queues.
    // Once a message is received, telemetry packet transfer frames
    // are created and sent to filter table task via message queue for
    // either immediate downlink or storage on secondary memory.
    while (1) {
        while (rx_msg_flg == 0) {
            // Check if housekeeping telemetry was received last loop. If
            // yes, then skip this loop to ensure magnetometer and imaging
            // message queue is checked:
            if (skip_hk_tlm_flg == 0) {
                // Read magnetometer DAQ source data from message queue
                // (if it is there):
                ret_val = rt_queue_read(&hk_tlm_msg_queue,\
                    &hk_tlm_buf,HK_TLM_SIZE,queue_timeout);

                // Check success:
                if (ret_val == HK_TLM_SIZE) {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Received housekeeping"
                        " telemetry\n",time(NULL));

                    // Set tlm_pkt_xfr_frm_apid:
                    tlm_pkt_xfr_frm_apid = APID_SW;

                    // Set flags:
                    rx_msg_flg = 1;      // Received message flag
                    skip_hk_tlm_flg = 1; // Skip checking magnetometer DAQ
                                         // source data availability next 
                                         // loop 
                // Timeout:
                } else if (ret_val == -ETIMEDOUT) {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Timed out waiting for"
                        " housekeeping telemetry; checking magnetometer"
                        " DAQ\n",time(NULL));
                // Error:
                } else {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Error receiving"
                        " housekeeping telemetry\n",time(NULL));
                    // NEED ERROR HANDLING                                          
                }
            }

            // Check if a message was just received. If yes, then skip checking
            // for magnetometer source data to create telemetry packets:
            if (rx_msg_flg == 0) {
                // Check if magnetometer DAQ data was received last loop. If
                // received, skip checking this loop to ensure that imaging
                // message queue is checked:
                if (skip_daq_src_flg == 0) {

                    // Read magnetometer DAQ source data from message queue
                    // (if it is there):
                    ret_val = rt_queue_read(&daq_src_dat_msg_queue,\
                        &daq_src_dat_buf,DAQ_SRC_DAT_SIZE,queue_timeout);

                    // Check success:
                    if (ret_val == DAQ_SRC_DAT_SIZE) {
                        // Print
                        rt_printf("%d (CRT_TLM_PKT_TASK) Received magnetometer"
                            " DAQ source data \n",time(NULL));

                        // Set tlm_pkt_xfr_frm_apid:
                        tlm_pkt_xfr_frm_apid = APID_MDQ;

                        // Set flags:
                        rx_msg_flg = 1;       // Received message flag
                        skip_daq_src_flg = 1; // Skip checking magnetometer DAQ
                                              // source data availability next 
                                              // loop 
                    // Timeout:
                    } else if (ret_val == -ETIMEDOUT) {
                        // Print
                        rt_printf("%d (CRT_TLM_PKT_TASK) Timed out waiting for"
                            " magnetometer DAQ source data; checking"
                            " imaging\n",time(NULL));
                    // Error:
                    } else {
                        // Print
                        rt_printf("%d (CRT_TLM_PKT_TASK) Error receiving"
                            " magnetometer DAQ source data\n",time(NULL));
                        // NEED ERROR HANDLING                                          
                    }
                }
            }

            // Check if a message was just received. If yes, skip checking for
            // imaging source data to create telemetry packets:
            if (rx_msg_flg == 0) {
                // Read imaging source data from message queue (if it's there):
                ret_val = rt_queue_read(&img_src_dat_msg_queue,\
                    &img_src_dat_buf,IMG_SRC_DAT_SIZE,queue_timeout);

                // Check success:
                if (ret_val == IMG_SRC_DAT_SIZE) {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Received imaging source" 
                        " data\n",time(NULL));

                    // Set tlm_pkt_xfr_frm_apid:
                    tlm_pkt_xfr_frm_apid = APID_IMG;
                    
                    // Set flags:
                    skip_hk_tlm_flg = 0;  // Don't skip checking magnetometer
                                          // DAQ source data availability next 
                                          // loop 
                    skip_daq_src_flg = 0; // Skip checking magnetometer DAQ
                                          // source data availability next 
                                          // loop 
                // Timeout:
                } else if (ret_val == -ETIMEDOUT) {
                    // Print
                    rt_printf("%d (CRT_TLM_PKT_TASK) Timed out waiting for"
                        " imaging source data; checking housekeeping"
                        " telemetry\n",time(NULL));

                    // Set flags:
                    skip_hk_tlm_flg = 0;  // Don't skip checking magnetometer
                                          // DAQ source data availability next 
                                          // loop 
                    skip_daq_src_flg = 0; // Skip checking magnetometer DAQ
                                          // source data availability next 
                                          // loop 
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
        if (tlm_pkt_xfr_frm_apid == APID_SW) {
            // Increment sequence count:
            tlm_pkt_xfr_frm_seq_cnt++;

            // Force counter roll over at 16383:
            // (the field in the packet that sequence occupies is only 14 bits)
            if (tlm_pkt_xfr_frm_seq_cnt == 16383) {
                tlm_pkt_xfr_frm_seq_cnt = 1; // 1 because it's logical (0 ain't)
            }

            // Set grouping flag:
            tlm_pkt_xfr_frm_grp_flg = 3; // Unsegmented data

            // Create transfer frame:
            crt_tlm_pkt_xfr_frm(hk_tlm_buf,HK_TLM_SIZE,\
                tlm_pkt_xfr_frm_buf,tlm_pkt_xfr_frm_apid,\
                tlm_pkt_xfr_frm_grp_flg,tlm_pkt_xfr_frm_seq_cnt);

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
        } else if (tlm_pkt_xfr_frm_apid == APID_MDQ) {
            // Increment sequence count:
            tlm_pkt_xfr_frm_seq_cnt++;

            // Force counter roll over at 16383:
            // (the field in the packet that sequence occupies is only 14 bits)
            if (tlm_pkt_xfr_frm_seq_cnt == 16383) {
                tlm_pkt_xfr_frm_seq_cnt = 1; // 1 because it's logical (0 ain't)
            }

            // Set grouping flag:
            tlm_pkt_xfr_frm_grp_flg = 3; // Unsegmented data

            // Create transfer frame:
            crt_tlm_pkt_xfr_frm(daq_src_dat_buf,DAQ_SRC_DAT_SIZE,\
                tlm_pkt_xfr_frm_buf,tlm_pkt_xfr_frm_apid,\
                tlm_pkt_xfr_frm_grp_flg,tlm_pkt_xfr_frm_seq_cnt);

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
                // Increment sequence count:
                tlm_pkt_xfr_frm_seq_cnt++;

                // Force counter roll over at 16383:
                // (the field in the packet that sequence occupies is only
                // 14 bits)
                if (tlm_pkt_xfr_frm_seq_cnt == 16383) {
                    tlm_pkt_xfr_frm_seq_cnt = 1; // 1 because it's logical
                                                 // (0 ain't)
                }

                // Set grouping flag based off iteration:
                if (i == 0) {
                    // Set grouping flag:
                    tlm_pkt_xfr_frm_grp_flg = 1; // First segment

                    // Set copy size in bytes:
                    copy_size = TLM_PKT_USR_DAT_SIZE;
                } else if (i == (num_tlm_pkt_xfr_frm_req - 1)) {
                    // Set grouping flag:
                    tlm_pkt_xfr_frm_grp_flg = 2; // Last segment

                    // Set copy size in bytes:
                    copy_size = IMG_SRC_DAT_SIZE - TLM_PKT_USR_DAT_SIZE*i;
                } else {
                    // Set grouping flag:
                    tlm_pkt_xfr_frm_grp_flg = 0; // Continuation segment

                    // Set copy size in bytes:
                    copy_size = TLM_PKT_USR_DAT_SIZE;
                }

                // Copy source data buffer to temporary buffer using dynamic
                // offset and size:
                memcpy(tmp_buffer,img_src_dat_buf+TLM_PKT_USR_DAT_SIZE*i,\
                    copy_size);

                // Create transfer frame:
                crt_tlm_pkt_xfr_frm(tmp_buffer,copy_size,\
                    tlm_pkt_xfr_frm_buf,tlm_pkt_xfr_frm_apid,\
                    tlm_pkt_xfr_frm_grp_flg,tlm_pkt_xfr_frm_seq_cnt);

                // Send transfer frame to filter table task via message queue:
                ret_val = rt_queue_write(&flt_tbl_msg_queue,\
                    &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                    Q_NORMAL); // Append message to queue

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
            }

            // Print:
            rt_printf("%d (CRT_TLM_PKT_TASK) All %d telemetry packet transfer"
                    " frame sent to filter table task\n",time(NULL),\
                    num_tlm_pkt_xfr_frm_req);
        }

        // Reset receive source data flag:
        rx_msg_flg = 0;
    }

    // Will never this reach this
    return;
}