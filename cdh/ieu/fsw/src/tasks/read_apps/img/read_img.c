///////////////////////////////////////////////////////////////////////////////
//
// Read Imaging
//
// Task responsible for reading data from imaging, interfacing with image
// processing software (IPS) via real-time message pipe (/dev/rtp0), creating
// telemetry packet transfer frames from processed images, and sending those
// frames to the filter table via message queue for either downlink or
// recording to storage for later downlink.
//
// The command imaging task notifies this task when a new image is taken and
// ready to be processed. The file "/tmp/img.raw" is assumed to be the location
// of this image and is read into a character byte array. The raw image is
// passed to IPS via real-time message pipe. The required reply from IPS is
// a 4 byte zero or non-zero number message that specifies
//     1. zero: image does not have an aurora in it so forget image
//     2. non-zero: image does have an aurora present so expect processed image
//                  of this non-zero number size
// If non-zero, the processed image is then received from IPS via the real-time
// message pipe. Once received, telemetry packet transfer frames are created
// and then sent to filter table task via message queue.
//
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - Xenomai 3 / Cobalt
// - IPS
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
#include <stdio.h>   // Standard input/output definitions
#include <unistd.h>  // UNIX standard function definitions
#include <errno.h>   // Error number definitions
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types

// Xenomai libraries:
#include <alchemy/task.h>  // Task management service
#include <alchemy/queue.h> // Message queue services
#include <alchemy/pipe.h>  // Message pipe services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <msg_queues.h>          // Message queue variable declarations
#include <msg_pipes.h>           // Message pipe variable declarations
#include <sems.h>                // Semaphore variable declarations
#include <crt_tlm_pkt_xfr_frm.h> // Create telemetry packet transfer frame
                                 // function declaration
#include <hk_tlm_var.h>          // Housekeeping telemetry variable
                                 // declarations

// Macro definitions:
#define RAW_IMG_SIZE      2304000 // Imaging source data message queue
                                  // size in bytes
#define TLM_PKT_XFR_FRM_SIZE 1089 // Telemetry transfer frame size in bytes
#define TLM_PKT_USR_DAT_SIZE 1064 // Telemetry packet user data size in bytes

#define APID_IMG 0x64 // Image origin

// Message queue definitions:
RT_QUEUE flt_tbl_msg_queue; // For telemetry packet transfer frames
                            // (read_mdq/img/get_hk_tlm --> flt_tbl_task)

// Message pipe declarations:
RT_PIPE ips_msg_pipe; // For raw and processed images
                      // (read_usb_img_task <--> ips)

// Semaphore definitions:
RT_SEM flt_tbl_sem; // For flt_tbl_task, read_mdq/img, and get_hk_tlm task
                    // synchronization
RT_SEM new_img_sem; // For run_cam_sgl and read_usb_img task
                    // synchronization  

// Global variable definitions:
uint16_t tlm_pkt_xfr_frm_seq_cnt = 0; // Packet sequence count
uint16_t img_accpt_cnt = 0;           // Accepted images (from IPS) count
uint16_t img_rej_cnt = 0;             // Rejected images (from IPS) count
uint8_t  ips_mdl_ld_state = 0;        // IPS model load state

void read_img(void) {
    // Print:
    rt_printf("%d (READ_IMG_TASK) Task started\n",time(NULL));

    // Task synchronize with filter table task:
    // (wait for task to be ready to telemetry packet transfer frames)
    rt_printf("%d (READ_IMG_TASK) Waiting for filter table task"
        " to be ready\n",time(NULL));

    // Wait for signals:
    rt_sem_p(&flt_tbl_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (READ_IMG_TASK) Filter table task is ready;"
        " continuing\n",time(NULL));

    // Definitions and initializations:
    uint32_t i;
    int32_t ret_val; // Function retern value

    uint16_t copy_size; // Buffer copy size

    uint8_t tlm_pkt_xfr_frm_grp_flg; // Packet sequence grouping flag

    uint16_t num_tlm_pkt_xfr_frm_req; // Required number of tranfer frames

    char tmp_buffer[TLM_PKT_USR_DAT_SIZE]; // Temporary buffer
    char tlm_pkt_xfr_frm_buf[TLM_PKT_XFR_FRM_SIZE]; // Buffer for telemetry
                                                    // packet transfer frame
                                                    // buffer

    FILE* file_ptr; // File pointer

    char* img_buf = (char*) malloc(RAW_IMG_SIZE); // Buffer for image

    uint32_t ips_ret; // IPS return value for passed raw image

    // Print:
    rt_printf("%d (READ_IMG_TASK) Waiting for IPS to be ready to receive"
        " and process images\n",time(NULL));

    // Synchronize with IPS:
    // (Wait IPS to be ready to receive and process images. Wait for message
    // indicating that IPS is ready to receive images via real-time message)
    ret_val = rt_pipe_read(&ips_msg_pipe,&ips_ret,1,TM_INFINITE);

    // Print:
    rt_printf("%d (READ_IMG_TASK) IPS is ready; continuing\n",time(NULL));

    // Set state:
    ips_mdl_ld_state = 1; // Ready

    // Print:
    rt_printf("%d (READ_IMG_TASK) Ready to process images, interface with"
        " IPS, and create telemetry packets\n",time(NULL));

    // Infinite loop to read images and send to create telemetry packets
    // task via message queue.
    while (1) {
        // Wait for new image to be created:
        rt_sem_p(&new_img_sem,TM_INFINITE);

        // Print:
        rt_printf("%d (READ_IMG_TASK) Reading new raw image\n",time(NULL));

        // Open file:
        file_ptr = fopen("/tmp/img.raw","rb");

        // Read raw image:
        fread(img_buf,RAW_IMG_SIZE,1,file_ptr);

        // Close file:
        fclose(file_ptr);

        // Send raw image to IPS via real-time message pipe:
        ret_val = rt_pipe_write(&ips_msg_pipe,img_buf,RAW_IMG_SIZE,\
            P_NORMAL);

        // Check success:
        if (ret_val == RAW_IMG_SIZE) {
            // Print:
            rt_printf("%d (READ_IMG_TASK) Sent raw image to IPS; waiting"
                " for reply\n",time(NULL));
        } else {
            rt_printf("%d (READ_IMG_TASK) Error sending raw image"
                " to IPS\n",time(NULL));

            // NEED ERROR HANDLING
        }

        // Read real-time message pipe for 4 byte reply from IPS:
        ret_val = rt_pipe_read(&ips_msg_pipe,&ips_ret,sizeof(ips_ret),\
            TM_INFINITE); 

        // Check IPS return. If 0, then image was not classified to have
        // an aurora in it so the image should be ignored. If non-zero,
        // then read pipe again for a processed image of that non-zero value
        // size and send image to create telemetry packets.
        if (ips_ret != 0) {
            // Print:
            rt_printf("%d (READ_IMG_TASK) Image classified to have an"
                " aurora by IPS; waiting for processed image\n",time(NULL));

            // Increment counter:
            img_accpt_cnt++;

            // Read real-time pipe for processed image:
            ret_val = rt_pipe_read(&ips_msg_pipe,img_buf,ips_ret,\
                TM_INFINITE); 
        
            // Check success:
            if (ret_val == ips_ret) {
                // Print:
                rt_printf("%d (READ_IMG_TASK) Received processed image"
                    " from IPS\n",time(NULL));
            } else {
                rt_printf("%d (READ_IMG_TASK) Error receiving processed"
                    " image from IPS\n",time(NULL));
                // NEED ERROR HANDLING
            }

            // Determine how many transfer frames are required:
            num_tlm_pkt_xfr_frm_req = \
                (ips_ret/TLM_PKT_USR_DAT_SIZE) + 1; 

            // Loop to create required number of transfer frames:
            for (i = 0; i < num_tlm_pkt_xfr_frm_req; ++i) {
                // Increment sequence count:
                tlm_pkt_xfr_frm_seq_cnt++;

                // Force counter roll over at 16384:
                // (the field in the packet that sequence occupies is only
                // 14 bits)
                if (tlm_pkt_xfr_frm_seq_cnt > 16383) {
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
                    copy_size = ips_ret - TLM_PKT_USR_DAT_SIZE*i;
                } else {
                    // Set grouping flag:
                    tlm_pkt_xfr_frm_grp_flg = 0; // Continuation segment

                    // Set copy size in bytes:
                    copy_size = TLM_PKT_USR_DAT_SIZE;
                }

                // Copy source data buffer to temporary buffer using dynamic
                // offset and size:
                memcpy(tmp_buffer,img_buf+TLM_PKT_USR_DAT_SIZE*i,\
                    copy_size);

                // Create transfer frame:
                crt_tlm_pkt_xfr_frm(tmp_buffer,copy_size,\
                    tlm_pkt_xfr_frm_buf,APID_IMG,\
                    tlm_pkt_xfr_frm_grp_flg,tlm_pkt_xfr_frm_seq_cnt);

                // Send transfer frame to filter table task via message queue:
                ret_val = rt_queue_write(&flt_tbl_msg_queue,\
                    &tlm_pkt_xfr_frm_buf,TLM_PKT_XFR_FRM_SIZE,\
                    Q_NORMAL); // Append message to queue

                // Check success:
                if ((ret_val > 0) || (ret_val == 0)) {
                    // Print:
                    //rt_printf("%d (READ_IMG_TASK) Telemetry packet transfer"
                    //    " frame sent to filter table task\n",time(NULL));
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
            }
            // Print:
            rt_printf("%d (READ_IMG_TASK) All %d telemetry packet transfer"
                " frames sent to filter table task\n",time(NULL),\
                num_tlm_pkt_xfr_frm_req);
        } else {
            // Print:
            rt_printf("%d (READ_IMG_TASK) Image classified to not have an"
                " aurora by IPS; ignoring image\n",time(NULL));

            // Increment counter:
            img_rej_cnt++;
        }
    }

    // Will never reach this:
    return;
}
