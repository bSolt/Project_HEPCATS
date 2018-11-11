///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Ground Control Command Macro Function
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - cmd_str
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
// Created: November 5, 2018
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

#include "gc_telecmd_inputs_struct.h" // Structure definition

#include "gc_cmd_str_interp.h"  // Function definition
#include "gc_crt_telecmd_pkt.h" // Function definition
#include "gc_open_port.h"       // Function definition
#include "gc_write_buffer.h"	 // Function definition

// "cmd" macro function 
void gc_macro_cmd(char* cmd_str)
{
	// Interpret command string:
	struct telecmd_pkt_inputs telecmd_pkt_inputs = gc_cmd_str_interp(cmd_str);

    // Create telecommand packet:
	char* buffer = malloc(20*sizeof(char));
    buffer = gc_crt_telecmd_pkt(telecmd_pkt_inputs,buffer);

    // Open port:
    int fd = gc_open_port("/dev/pts/2");

    // Write buffer to port:
    gc_write_buffer(fd, buffer); 

    // Close port:
    close(fd);

	return;
}