///////////////////////////////////////////////////////////////////////////////
//
// Serial Communication Test
//
// Read main
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
// ASEN 4018
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

void main(int argc, char const *argv[])
{
	// Declaration:
	int bytes_received;
	unsigned char buffer[11];

	uint8_t h1; // 1 byte
	uint8_t h2; // 1 byte
	uint8_t d1; // 1 byte

	// Define port name:
	char* port = "/dev/pts/3"; // Will have to update!!

	// Open port:
	int fd = open_port(port);

	// Read data from port:
	do {
		// Read:
		bytes_received = read(fd, &buffer, sizeof(buffer));

		// If bytes are received:
		if (bytes_received  == 3) {
			// Extract data from the buffer:
			memcpy(&h1,buffer,1);
			memcpy(&h2,buffer+1,1);
			memcpy(&d1,buffer+2,1);

			// Print to the screen:
			printf("Packet received!\n");
			printf("(%d bytes received)\n",bytes_received);
			printf("----------------------------\n");
			printf("Raw\n");
			printf("  %s\n",buffer);
			printf("Converted\n");
			printf("  Header H1:  %d\n",h1);
			printf("  Header H2:  %d\n",h2);
			printf("  Data   D1:  %d\n",d1);
			printf("----------------------------\n");
			printf("\n");
		} 
		else if (bytes_received != 3) {
			printf("Error from read: %d: %s\n", bytes_received, strerror(errno));
		}

	// Repeat to get full message:
	} while (1);

  	// Closes port:
  	close(fd);

	return;
}