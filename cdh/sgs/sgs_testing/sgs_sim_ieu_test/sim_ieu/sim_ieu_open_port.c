///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
//
// Simulated IEU Open Port
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - port 
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

#include "sim_ieu_port_config.h"

int sim_ieu_open_port(char* port)
{
  // File descriptor for the port:
  int fd;

  // Open port:
  fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

  // Check for success 
  if (fd == -1){
    // Could not open port:
    perror("open_port: Unable to open port - ");

    // Exit:
    exit(0);
  }
  else{
    // Set file status:
    fcntl(fd, F_SETFL, 0);
  }

  // Set speed to  bps, 8n1 (no parity)
  sim_ieu_port_config(fd,B115200); 

  // Return fd:
  return (fd);
}
