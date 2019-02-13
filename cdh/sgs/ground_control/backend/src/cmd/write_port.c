///////////////////////////////////////////////////////////////////////////////
//
// Write (Serial) Port
//
// Write telecommand packet to uplink serial port
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - fd
// - buffer
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
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <errno.h>   // Error number definitions 

// Write buffer to port function
void write_port(int fd, char* buffer) {
    // Write buffer to port:
    int bytes_sent = write(fd,buffer,20); // 17 byte telecommand packet

    // Check for success (20 bytes sent):
    if (bytes_sent != 20) {
        // Print error message:
        printf("(WRITE_BUFFER) <ERROR> Unable to write: %d, %d\n",\
            bytes_sent,errno);
    } else {
    	// Print:
    	printf("(WRITE_BUFFER) Command sent\n");
    }

    return;
}