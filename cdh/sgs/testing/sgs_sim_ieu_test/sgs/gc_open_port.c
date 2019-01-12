///////////////////////////////////////////////////////////////////////////////
//
// Serial Communication Test
//
// Ground Control Open Port
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
// Created: October 25, 2018
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
#include "gc_config_port.h"

int gc_open_port(char* port) {
  // File descriptor for the port:
  int fd;

  // Open port:
  fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

  // Check for success 
  if (fd == -1) {
    // Could not open port:
    printf("(GC_OPEN_PORT) <ERROR> Unable to open port: %d\n",errno);

    // Exit:
    exit(0);
  } else{
    // Set file status:
    fcntl(fd, F_SETFL, 0);
  }

  // Set speed to  bps, 8n1 (no parity)
  gc_config_port(fd,B115200); 

  // Return fd:
  return (fd);
}
