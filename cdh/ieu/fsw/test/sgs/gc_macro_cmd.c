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

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <stdint.h>  // Standard integer types
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions

// Header files:
#include "gc_telecmd_inputs_struct.h" // Structure declaration
#include "gc_interp_cmd_str.h"        // Interpret command string function declaration
#include "gc_crt_telecmd_pkt.h"       // Create packet function declaration
#include "gc_open_port.h"             // Open port function declaration
#include "gc_write_port.h"            // Write port function declaration

void gc_macro_cmd(char* cmd_str_arr[]) {
    // Interpret command string:
    struct telecmd_pkt_inputs telecmd_pkt_inputs = \
        gc_interp_cmd_str(cmd_str_arr);

    // Create telecommand packet:
    char* buffer = malloc(20*sizeof(char));
    buffer = gc_crt_telecmd_pkt(telecmd_pkt_inputs,buffer);

    // Open port:
    int fd = gc_open_port("/dev/pts/4");

    // Write buffer to port:
    gc_write_port(fd, buffer);

    // Close port:
    close(fd);
    
    return;
}
