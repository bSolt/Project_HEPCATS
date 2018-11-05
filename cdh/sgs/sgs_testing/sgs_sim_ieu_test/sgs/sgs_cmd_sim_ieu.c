///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// SGS Command Simulated IEU
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

#include "telecmd_inputs_struct.h" // Structure definition

#include "cmd_str_interp.h"  // Function definition
#include "crt_telecmd_pkt.h" // Function definition
#include "open_port.h"       // Function definition
#include "write_buffer.h"	 // Function definition

void main(int argc, char const *argv[])
{
	char cmd_str_1[] = "cmd hepcats noop";
	char cmd_str_2[] = "cmd hepcats noop with rsp";	
	char cmd_str_3[] = "cmd hepcats noop hold 2018/320-00:00:00";
	char cmd_str_4[] = "cmd hepcats noop with rsp hold 2018/320-00:00:00";

	// Interpret command string:
	struct telecmd_pkt_inputs telecmd_pkt_inputs = cmd_str_interp(cmd_str_1);

    // Create telecommand packet:
	char* buffer = malloc(20*sizeof(char));
    buffer = crt_telecmd_pkt(telecmd_pkt_inputs,buffer);

    // Open port:
    int fd = open_port("/dev/pts/2");

    // Write buffer to port:
    write_buffer(fd, buffer); 

    // Close port:
    close(fd);

	return;
}