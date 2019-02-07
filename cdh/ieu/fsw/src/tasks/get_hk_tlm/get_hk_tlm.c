///////////////////////////////////////////////////////////////////////////////
//
// Get Housekeeping (HK) Telemetry
//
// Task responsible for retrievig values of counters (housekeeping telemetry)
// and sending it to create telemetry packet task via message queue. This is
// done be reading variables defined in the housekeeping variable declaration
// header file.
//
// This is a periodic task with a frequency set during task creation. The
// frequency should nominally be 1 Hz (once every second).
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
#include <alchemy/task.h>  // Task management serives
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services
#include <alchemy/timer.h> // Timer management services

// Header files:
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations
#include <hk_tlm_var.h> // Housekeeping telemetry variable declarations

// Macro definitions:
#define HK_TLM_SIZE 7 // Housekeeping telemetry message queue size in bytes

// Message queue definitions:
RT_QUEUE hk_tlm_msg_queue; // For housekeeping telemetry
                           // (get_hk_tlm_task --> crt_tlm_pkt_task)

// Semaphore definitions:
RT_SEM crt_tlm_pkt_sem; // For crt_tlm_pkt_task and read_usb tasks
                        // synchronization

// Definitions for housekeeping telemetry variables:
uint8_t val_telecmd_pkt_cnt; // Valid telecommand packet counter
uint8_t inv_telecmd_pkt_cnt; // Invalid telecommand packet counter
uint8_t rx_telecmd_pkt_cnt;  // Received telecommand packet count
uint8_t val_cmd_apid_cnt;    // Valid command counter
uint8_t inv_cmd_apid_cnt;    // Invalid command counter
uint8_t cmd_exec_suc_cnt;    // Commands executed successfully counter
uint8_t cmd_exec_err_cnt;    // Commands not executed (error) counter

void get_hk_tlm(void* arg){
    // Print:
    rt_printf("%d (GET_HK_TLM_TASK) Task started\n",time(NULL));

    // Task synchronize with create telemetry packet:
    // (wait for task to be ready to receive source data)
    rt_printf("%d (GET_HK_TLM_TASK) Waiting for create telemetry packet task"
        " to be ready\n",time(NULL));

    // Wait for signals:
    rt_sem_p(&crt_tlm_pkt_sem,TM_INFINITE);

    // Print:
    rt_printf("%d (GET_HK_TLM_TASK) Create telemetry packet task is" 
        " ready; continuing\n",time(NULL));

    // Definitions and initializations:
    int8_t ret_val; // Function return value

    char hk_tlm_buf[HK_TLM_SIZE]; // Buffer housekeeping telemetry

    // Print 
    rt_printf("%d (GET_HK_TLM_TASK) Ready to get housekeeping telemetry\n",\
        time(NULL));

    // Infinite loop to get housekeeping telemetry and send to create
    // telemetry packet task via message queue:
    while (1) {
        // Copy housekeeping telemetry to buffer:
        memcpy(hk_tlm_buf,&rx_telecmd_pkt_cnt,1);
        memcpy(hk_tlm_buf+1,&val_telecmd_pkt_cnt,1);
        memcpy(hk_tlm_buf+2,&inv_telecmd_pkt_cnt,1);
        memcpy(hk_tlm_buf+3,&val_cmd_apid_cnt,1);
        memcpy(hk_tlm_buf+4,&inv_cmd_apid_cnt,1);
        memcpy(hk_tlm_buf+5,&cmd_exec_suc_cnt,1);
        memcpy(hk_tlm_buf+6,&cmd_exec_err_cnt,1);

        // Send source data to create telemetry packet task via message queue:
        ret_val = rt_queue_write(&hk_tlm_msg_queue,&hk_tlm_buf,\
            HK_TLM_SIZE,Q_NORMAL); // Append message to queue

        // Check success:
        if ((ret_val > 0) || (ret_val == 0)) {
            // Print:
            rt_printf("%d (GET_HK_TLM_TASK) Housekeeping telemetry"
                " sent to create telemetry packet task\n",time(NULL));
        } else {
            rt_printf("%d (GET_HK_TLM_TASK) Error sending"
                " housekeeping telemetry to create telemetry packet task\n",\
                time(NULL));
            // NEED ERROR HANDLING
        }

        // Release processor and wait for next period to execute again:
        rt_task_wait_period(NULL);
    }
}