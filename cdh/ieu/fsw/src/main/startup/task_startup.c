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

// Create tasks
void crt_tasks(void) {
    // Print:
    rt_printf("%d (STARTUP/CRT_TASKS) Creating all tasks\n",time(NULL));

    // Create tasks:
    rt_task_create(&rx_telecmd_pkt_task,"rx_telecmd_pkt_task",0,50,T_JOINABLE);
    rt_task_create(&proc_telecmd_pkt_task,"proc_telecmd_pkt_task",0,50,0);
    rt_task_create(&exec_cmd_task,"exec_cmd_task",0,50,0);
    rt_task_create(&sched_cmd_task,"sched_cmd_task",0,50,0);
    rt_task_create(&cmd_sw_task,"cmd_sw",0,50,0);
    rt_task_create(&cmd_img_task,"cmd_img",0,50,0);
    rt_task_create(&cmd_mdq_task,"cmd_mdq",0,50,0);
    rt_task_create(&cmd_ers_task,"cmd_ers",0,50,0);

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

    // Print:
    rt_printf("%d (STARTUP/STR_TASKS) All tasks started\n",time(NULL));

    return;
}