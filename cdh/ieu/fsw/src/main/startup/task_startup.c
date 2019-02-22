///////////////////////////////////////////////////////////////////////////////
//
// Tasks Startup
//
// Functions defined in this file create Xenomai tasks and start them as a
// part of flight software initialization. Tasks are then executed by the
// scheduler based off given scheduling rules.
//
// These functions are called by startup().
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
// Created: January 12, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <time.h> // Standard time types

// Xenomai libraries:
#include <alchemy/task.h> // Task management services

// Header files:
#include <tasks.h> // Task variable and function declarations

// Macro definitions:
#define GET_HK_TLM_FREQ 1e9 // Housekeeping telemetry frequency in seconds

// Task definitions:
RT_TASK rx_telecmd_pkt_task;   // Receive telecommand packet from uplink
                               // serial port
RT_TASK proc_telecmd_pkt_task; // Process telecommand packet
RT_TASK exec_cmd_task;         // Execute command
RT_TASK sched_cmd_task;        // Command scheduler
RT_TASK cmd_sw_task;           // Execute software command
RT_TASK cmd_img_task;          // Execute imaging command
RT_TASK cmd_mdq_task;          // Execute magnetometer DAQ command
RT_TASK cmd_ers_task;          // Execute electrical relay switch command
RT_TASK crt_tlm_pkt_task;      // Create telemetry packet
RT_TASK read_usb_daq_task;     // Read magnetometer DAQ
RT_TASK read_usb_img_task;     // Read camera
RT_TASK flt_tbl_task;          // (Telemetry) Filter table
RT_TASK tx_tlm_pkt_task;       // Transmit telemetry packet to downlink
                               // serial port
RT_TASK crt_file_task;         // Create file
RT_TASK rtrv_file_task;        // Retrieve file for downlink
RT_TASK get_hk_tlm_task;       // Get housekeeping telemetry

// Create tasks
void crt_tasks(void) {
    // Print:
    rt_printf("%d (STARTUP/CRT_TASKS) Creating all tasks\n",time(NULL));

    // Create tasks:
    rt_task_create(&rx_telecmd_pkt_task,"rx_telecmd_pkt_task",0,80,T_JOINABLE);
    rt_task_create(&proc_telecmd_pkt_task,"proc_telecmd_pkt_task",0,80,0);
    rt_task_create(&exec_cmd_task,"exec_cmd_task",0,80,0);
    rt_task_create(&sched_cmd_task,"sched_cmd_task",0,85,0);
    rt_task_create(&cmd_sw_task,"cmd_sw_task",0,90,0);
    rt_task_create(&cmd_img_task,"cmd_img_task",0,90,0);
    rt_task_create(&cmd_mdq_task,"cmd_mdq_task",0,90,0);
    rt_task_create(&cmd_ers_task,"cmd_ers_task",0,90,0);
    rt_task_create(&read_usb_daq_task,"read_usb_daq_task",0,40,0);
    rt_task_create(&read_usb_img_task,"read_usb_img_task",0,40,0);
    rt_task_create(&get_hk_tlm_task,"get_hk_tlm_task",0,40,0);
    rt_task_create(&flt_tbl_task,"flt_tbl_task",0,60,0);
    rt_task_create(&tx_tlm_pkt_task,"tx_tlm_pkt_task",0,70,0);
    rt_task_create(&crt_file_task,"crt_file_task",0,70,0);
    rt_task_create(&rtrv_file_task,"rtrv_file_task",0,80,0);

    // Set periodic mode on select tasks:
    rt_task_set_periodic(&get_hk_tlm_task,TM_NOW,GET_HK_TLM_FREQ); // Set NOW

    // Print:
    rt_printf("%d (STARTUP/CRT_TASKS) All tasks created\n",time(NULL));

    return;
}

// Start tasks
void str_tasks(void){
    // Print:
    rt_printf("%d (STARTUP/STR_TASKS) Starting all tasks\n",time(NULL));

    // Start tasks:
    rt_task_start(&rx_telecmd_pkt_task,&rx_telecmd_pkt,0);
    rt_task_start(&proc_telecmd_pkt_task,&proc_telecmd_pkt,0);
    rt_task_start(&exec_cmd_task,&exec_cmd,0);
    rt_task_start(&sched_cmd_task,&sched_cmd,0);
    rt_task_start(&cmd_sw_task,&cmd_sw,0);
    rt_task_start(&cmd_img_task,&cmd_img,0);
    rt_task_start(&cmd_mdq_task,&cmd_mdq,0);
    rt_task_start(&cmd_ers_task,&cmd_ers,0);
    rt_task_start(&read_usb_daq_task,&read_usb_daq,0);
    rt_task_start(&read_usb_img_task,&read_usb_img,0);
    rt_task_start(&flt_tbl_task,&flt_tbl,0);
    rt_task_start(&tx_tlm_pkt_task,&tx_tlm_pkt,0);
    rt_task_start(&crt_file_task,&crt_file,0);
    rt_task_start(&rtrv_file_task,&rtrv_file,0);
    rt_task_start(&get_hk_tlm_task,&get_hk_tlm,0);

    // Print:
    rt_printf("%d (STARTUP/STR_TASKS) All tasks started\n",time(NULL));

    return;
}