///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
//
// Ground Control Receive Telemetry
//
// This script continually reads port for telemetry packets. If a telemetry
// packet is read from the port, it is then processed.
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
// Created: November 10, 2018
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Standard integer types

#include "gc_open_port.h"    // Open port function declaration
#include "gc_proc_tlm_pkt.h" // Process packet function declaration
#include "gc_read_port.h"    // Read port function declaration

void main(int argc, char const *argv[]) {
    // Open port:
    int fd = gc_open_port("/dev/pts/3");

    // Allocate buffer:
    char* buffer = malloc(1080*sizeof(char));
        
    // Read from port forever:
    while(1) {
        // Read from port:
        buffer = gc_read_port(fd,buffer);

        // Process telemetry packet:
        gc_proc_tlm_pkt(buffer);
    }

    // Will never reach this:
    return;
}
