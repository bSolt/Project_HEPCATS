///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
//
// Simulated IEU Receive Ground Telecommand
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

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <stdint.h>  // Standard integer types

// Header files:
#include "sim_ieu_open_port.h"        // Open port function declarations
#include "sim_ieu_read_port.h"      // Read buffer function declarations
#include "sim_ieu_proc_telecmd_pkt.h" // Process packet function declarations

void main(int argc, char const *argv[]) {
    // Open port:
    int fd = sim_ieu_open_port("/dev/pts/3");

    // Read from buffer:
    char* buffer = malloc(17*sizeof(char));
    buffer = sim_ieu_read_port(fd,buffer);

    // Process telecommand packet:
    sim_ieu_proc_telecmd_pkt(buffer);

    // Close port:
    close(fd);

    return;
}
