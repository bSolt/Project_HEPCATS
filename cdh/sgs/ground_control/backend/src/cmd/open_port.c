///////////////////////////////////////////////////////////////////////////////
//
// Open (Serial) Port
//
// Open serial uplink port
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - port 
//
// Output Arguments:
// - fd
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
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions

// Header files:
#include "config_port.h"

int open_port(char* port) {
  // File descriptor for the port:
  int fd;

  // Open port:
  fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

  // Check for success 
  if (fd == -1) {
    // Could not open port:
    printf("(OPEN_PORT) <ERROR> Unable to open port: %d\n",errno);

    // Exit:
    exit(0);
  } else{
    // Set file status:
    fcntl(fd, F_SETFL, 0);

    // Print:
    printf("(OPEN_PORT) Uplink serial port opened\n");
  }

  // Set speed to  bps, 8n1 (no parity)
  config_port(fd,B1000000); 

  // Return fd:
  return (fd);
}
