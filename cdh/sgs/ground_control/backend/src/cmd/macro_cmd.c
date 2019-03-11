///////////////////////////////////////////////////////////////////////////////
//
// Macro Command
//
// Interpret prompt input that invokes the "cmd" macro
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
// Created: January 12, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <stdint.h>  // Integer types

// Header files:
#include "telecmd_inputs_struct.h" // Structure declaration
#include "interp_cmd_str.h"        // Interpret command string function declaration
#include "crt_telecmd_pkt.h"       // Create packet function declaration
#include "open_port.h"             // Open port function declaration
#include "write_port.h"            // Write port function declaration

void macro_cmd(char* cmd_str_arr[]) {
    // Interpret command string:
    struct telecmd_pkt_inputs telecmd_pkt_inputs = \
        interp_cmd_str(cmd_str_arr);

    // Create telecommand packet:
    char* buffer = malloc(20*sizeof(char));
    buffer = crt_telecmd_pkt(telecmd_pkt_inputs,buffer);

    // Open port:
    int fd = open_port("/dev/ttyUSB0");

    // Write buffer to port:
    write_port(fd, buffer);

    // Close port:
    close(fd);
    
    return;
}
