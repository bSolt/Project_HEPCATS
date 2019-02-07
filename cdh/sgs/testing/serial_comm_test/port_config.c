///////////////////////////////////////////////////////////////////////////////
//
// Serial Communication Test
//
// Function: Sets port configuration (options)
//           - Sets baud rate
//           - Sets parity
//           - Sets to raw input
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - fd (file descriptor)
// - speed (baud rate)
// - parity (even, odd, or none)
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

void port_config(int fd, int speed) {
    // Creat POSIX structure for port options:
    struct termios options;

    // Get the current options for the port:
    tcgetattr(fd,&options);

    // Set baud rates:
    cfsetospeed(&options,speed);
    cfsetispeed(&options,speed);

    // Mask the character size bits:
    options.c_cflag &= ~CSIZE;

    // Select 8 data bits for 1 character:
    options.c_cflag |= CS8; 

    // No parity:
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Set to raw input with 1 second timeout:
    // Options are good for modem
    options.c_cflag     |= (CLOCAL | CREAD);
    options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag     &= ~OPOST;
    options.c_cc[VMIN]  = 0;  // Minimum number of characters to read
    options.c_cc[VTIME] = 5;  // Time to wait for every character to be 
                              // read [tenths of seconds]

    // Set the options now:
    tcsetattr(fd,TCSANOW,&options);

    return;
}