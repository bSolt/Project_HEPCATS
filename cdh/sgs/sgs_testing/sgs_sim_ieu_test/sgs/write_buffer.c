///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Write Buffer
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

void write_buffer(int fd, char* buffer)
{
	// Write buffer to port:
	int bytes_sent = write(fd,buffer,sizeof(buffer));

	// Check for success:
	if (bytes_sent != sizeof(buffer)) {
		// Print error message:
	    printf("Error from write: %d, %d\n", bytes_sent, errno);
	} else {
		// Print success message:
		printf("Telecommand Packet Sent\n");
	}

	return;
}