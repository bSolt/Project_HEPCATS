///////////////////////////////////////////////////////////////////////////////
//
// Receive Telemetry
//
// Receive telemetry packets from IEU via downlink serial port
//
// This script continually reads port for telemetry packets. If a telemetry
// packet is read from the port, it is then processed and then either printed
// to the screen or saved as binary files.
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
// Created: February 12, 2018
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Standard integer types

#include "open_port.h"    // Open port function declaration
#include "proc_tlm_pkt.h" // Process packet function declaration
#include "read_port.h"    // Read port function declaration

void main(int argc, char const *argv[]) {
    // Open port:
    int fd = open_port("/dev/pts/2");

    // Allocate buffer for telemetry packet:
    char* buf = malloc(1080*sizeof(char));

    // Read from port forever:
    while(1) {
        // Read from port:
        buf = read_port(fd,buf);

        // Process telemetry packet:
        proc_tlm_pkt(buf);
    }

    // Will never reach this:
    return;
}
