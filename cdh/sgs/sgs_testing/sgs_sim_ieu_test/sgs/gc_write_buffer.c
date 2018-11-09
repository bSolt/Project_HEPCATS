///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Ground Control Write (to) Buffer
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
// Created: November 4, 2018
// 
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <stdint.h>  // Integer types
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <fcntl.h>   // File control definitions 
#include <errno.h>   // Error number definitions 
#include <termios.h> // POSIX terminal control definitions 

void gc_write_buffer(int fd, char* buffer)
{
	// Write buffer to port:
	int bytes_sent = write(fd,buffer,20); // 20 byte telecommand packet

	// Check for success (20 bytes sent):
	if (bytes_sent != 20) {
		// Print error message:
	    printf("(GC_WRITE_BUFFER) <ERROR> Unable to write: %d, %d\n",\
    		bytes_sent,errno);
	}

	return;
}