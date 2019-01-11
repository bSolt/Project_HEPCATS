///////////////////////////////////////////////////////////////////////////////
//
// Open Port
//
// Opens input serial port at input speed (baud rate)
//
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - COM Port
//
// Input Arguments:
// - port
// - speed
//
// Output Arguments:
// - fd (file descriptor)
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: January 7, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions

// Xenomai libraries:
#include <alchemy/task.h>

// Header files:
#include <config_port.h> // Config port function declaration

int open_port(char* port, int speed) {
    // Initialize:
    int8_t fd;      // File descriptor
    int8_t ret_val; // Function return value

    // Open port:
    fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

    // Check for success
    if (fd > 0){
        // Set file status:
        ret_val = fcntl(fd,F_SETFL,0);

        // Check failure:
        if (ret_val < 0) {
    	   // NEED ERROR HANDLING:
        }
    } else {
        // NEED ERROR HANDLING:
    }

    // Configure port:
    config_port(fd,speed);

    // Return fd:
    return fd;
}
