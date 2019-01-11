///////////////////////////////////////////////////////////////////////////////
//
// Main
//
// This is the main function for flight software. It is responsible for booting
// up flight software. Specifically, it creates
//     - Message queues and pipes
//     - semaphores
//     - tasks
// Once every task has been created, they are started. Main then waits for the
// termination of the last task it starts. Since this task will never
// terminate, main is blocked in the non real-time context.
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
// Created: January 6, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>  // Standard input/output definitions
#include <stdlib.h> // Standard library
#include <unistd.h> // UNIX standard function definitions
#include <time.h>   // Standard time types

// Xenomai libraries:
#include <alchemy/task.h>  // Task management services
#include <alchemy/queue.h> // Message queue services
#include <alchemy/sem.h>   // Semaphore services

// Header files:
#include <tasks.h>      // Task variable and function declarations
#include <msg_queues.h> // Message queue variable declarations
#include <sems.h>       // Semaphore variable declarations

// Message queue definitions:
RT_QUEUE telecmd_msg_queue; // For command transfer packets
                            // (rx_telecmd_pkt_task --> proc_telecmd_pkt_task)

// Semaphore definitions:
RT_SEM telecmd_pkt_sem; // For rx_telecmd_pkt_task and proc_telecmd_pkt_task
                        // task synchronization

// Task definitions:
RT_TASK rx_telecmd_pkt_task;   // Receive telecommand packet
RT_TASK proc_telecmd_pkt_task; // Process telecommand packet

void crt_msg_queues_pipes(){
    // Print:
    rt_printf("%d (STARTUP/CRT_MSG_QUEUES_PIPES)"
        " Creating message queues and pipes\n",time(NULL));

    // Create message queues:
    rt_queue_create(&telecmd_msg_queue,\
        "telecmd_msg_queue",13,10,Q_FIFO); // 13 bytes per message
                                           // Maximum of 10 messages in queue

    // Print:
    rt_printf("%d (STARTUP/CRT_MSG_QUEUES_PIPES)"
        " Message queues and pipes created\n",time(NULL));
}

void crt_sems(){
    // Print:
    rt_printf("%d (STARTUP/CRT_SEMS)"
        " Creating semaphores\n",time(NULL));

    // Create semaphores:
    rt_sem_create(&telecmd_pkt_sem,"telecmd_pkt_sem",0,S_FIFO);

    // Print:
    rt_printf("%d (STARTUP/CRT_SEMS)"
        " Semaphores created\n",time(NULL));
}

void crt_tasks(void){
    // Print:
    rt_printf("%d (STARTUP/CRT_TASKS) Creating all tasks\n",time(NULL));

    // Create tasks:
    rt_task_create(&rx_telecmd_pkt_task,"rx_telecmd_pkt_task",0,50,0);
    rt_task_create(&proc_telecmd_pkt_task,"proc_telecmd_pkt_task",0,50,0);

    // Print:
    rt_printf("%d (STARTUP/CRT_TASKS) All tasks created\n",time(NULL));

    return;
}

void str_tasks(void){
    // Print:
    rt_printf("%d (STARTUP/STR_TASKS) Starting all tasks\n",time(NULL));

    // Start tasks:
    rt_task_start(&rx_telecmd_pkt_task,&rx_telecmd_pkt,0);
    rt_task_start(&proc_telecmd_pkt_task,&proc_telecmd_pkt,0);

    // Print:
    rt_printf("%d (STARTUP/STR_TASKS) All tasks started\n",time(NULL));

    return;
}

void startup(void){
    // Print:
    rt_printf("%d (STARTUP) Flight software boot-up started\n",time(NULL));

    // Create message queues and pipes:
    crt_msg_queues_pipes();

    // Create semaphores:
    crt_sems();

    // Create tasks:
    crt_tasks();

    // Start tasks:
    str_tasks();

    // Print:
    rt_printf("%d (STARTUP) Flight software boot-up complete\n",time(NULL));
}

int main(void) {
    // Startup:
    startup();

    pause();

    // This will never be reached
    return 0;
}