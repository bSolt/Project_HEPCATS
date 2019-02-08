///////////////////////////////////////////////////////////////////////////////
//
// Main
//
// This is the main function for flight software. It is responsible for 
// initializing flight software by calling the function startup(). This 
// function creates Xenomai data structures (message queues, pipes,
// semaphores, etc.), creates Xenomai tasks, and then starts these tasks.
// The scheduler than executes these task based on given scheduling rules.
//
// Main then waits for a task to terminate. Since this task, or any
// task for that matter, will never terminate, main is blocked in the non 
// real-time context forever.
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

// Xenomai libraries:
#include <alchemy/task.h> // Task management services

// Header files:
#include <startup.h> // Startup (flight software initialization) function 
                     // declaration
#include <tasks.h>   // Task variable and function declarations

// Task definitions:
RT_TASK rx_telecmd_pkt_task; // Receive telecommand packet

int main(void) {
    // Startup:
    startup();

    // Wait for task to terminate:
    // (task will never terminate so this will cause main to be blocked
    // in the non real-time context forever)
    rt_task_join(&rx_telecmd_pkt_task);

    // This will never be reached
    return 0;
}