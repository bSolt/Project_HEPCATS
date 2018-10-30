///////////////////////////////////////////////////////////////////////////////
//
// Serial Communication Test
//
// Write main
//
// -------------------------------------------------------------------------- /
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
// ASEN 4013
// Project HEPCATS
// Subsystem: C&DH
// Created: October 25, 2018
// 
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <fcntl.h>   // File control definitions 
#include <errno.h>   // Error number definitions 
#include <termios.h> // POSIX terminal control definitions 
#include <stdint.h>  // Integer types

#include "open_port.h"

// Define structure for an example packet:
struct header {
    uint8_t h1;   // 1 byte 
    uint8_t h2;  // 1 bytes
};
struct data {
    uint8_t d1; // 1 bytes
};

void main(int argc, char const *argv[])
{
	// Declarations:
	int bytes_sent;
	unsigned char buffer[3]; // Buffer 3 bytes in size

	// Define port name:
	char* port = "/dev/pts/1"; // Will have to update!!

	// Open and configure port:
	int fd = open_port(port);

	// Declare structures:
	struct header header;
	struct data data;

	// Fill in data for structure:
	header.h1 = 250;
	header.h2 = 150;
	data.d1 = 31;

	// Write data to port:
	do {
		// Change data:
		header.h1 = header.h1 + 1;
		header.h2 = header.h2 + 5;
		data.d1 = data.d1 + 2;

		// Build buffer:
		memcpy(buffer+0,&header.h1,1);
		memcpy(buffer+1,&header.h2,1);
		memcpy(buffer+2,&data.d1,1);

		bytes_sent = write(fd,buffer,sizeof(buffer));

		// Check for success:
		if (bytes_sent != sizeof(buffer)) {
		    printf("Error from write: %d, %d\n", bytes_sent, errno);
		}
		else {
			printf("Packet sent!\n");
			printf("(%d bytes sent)\n",bytes_sent);
			printf("\n");
		}

		// Waits for 2 seconds
		sleep(5);

	} while(1); // Continues indefinitely

  	// Closes port:
  	close(fd);
	
	return;
}