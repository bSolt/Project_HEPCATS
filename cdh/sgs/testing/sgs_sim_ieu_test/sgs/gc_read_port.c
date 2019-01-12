///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Read port for telemetry packet
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - fd
// - buffer
//
// Output Arguments:
// - buffer
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

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <stdint.h>  // Standard integer types
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions

char* gc_read_port(int fd,char* buffer) {
	// Definitions and initializations: 
	int bytes_received;
	int bytes_read = 0;
	int bytes_to_read = 1080; // 20 bytes telecommand packet

	// Temporary buffer:
	char temp_buffer[1080];

	// Loop to get full telecommand packet:
	while (bytes_to_read > 0 ) {
		// Read from port:
		bytes_received = read(fd,&temp_buffer[bytes_read],bytes_to_read);

		// Check to see if entire packet is received:
		if ( bytes_received > 0 ) {
			// Increment bytes read:
			bytes_read += bytes_received;

			// Increment bytes to read:
			bytes_to_read -= bytes_received;
		} 
	}	

	// Copy temporary buffer to return buffer:
	memcpy(buffer,&temp_buffer,1080);

	// Return:
  	return buffer;
}