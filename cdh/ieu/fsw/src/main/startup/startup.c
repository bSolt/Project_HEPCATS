///////////////////////////////////////////////////////////////////////////////
//
// Startup (flight software initialization)
//
// This is the startup function for flight software. It is responsible for 
// initializing flight software. It creates
//     - Xenomai data structures (message queues, pipes, semaphores, etc.)
//     - Xenomai tasks
// Once tasks have been created, they are started and scheduled to execute by
// the scheduler given set scheduling rules.
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
#include <time.h> // Time and date

// Xenomai libraries:
#include <alchemy/task.h> // Task management services

// Header files:
#include <dat_struct_startup.h> // Create Xenomai data structures function 
                                // declarations 
#include <task_startup.h>       // Create and start Xenomai task function 
                                // declarations

void startup(void) {
    // Print:
    rt_printf("%d (STARTUP) Flight software initialization started\n",time(NULL));

    // Create message queues and pipes:
    crt_msg_queues_pipes();

    // Create semaphores:
    crt_sems();

    // Create tasks:
    crt_tasks();

    // Start tasks:
    str_tasks();

    // Print:
    rt_printf("%d (STARTUP) Flight software initialization complete\n",time(NULL));

    return;
}