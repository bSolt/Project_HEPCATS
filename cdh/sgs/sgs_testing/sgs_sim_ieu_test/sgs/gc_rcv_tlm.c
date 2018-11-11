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

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Integer types

#include "gc_open_port.h"    // Function definition
#include "gc_tlm_pkt_proc.h" // Function definition
#include "gc_read_buffer.h"

void main(int argc, char const *argv[])
{
	// Open port:
    int fd = gc_open_port("/dev/pts/2");

    // Allocate buffer:
    char* buffer = malloc(1080*sizeof(char));
        
    // Read from port forever:
    do {
    	// Read from port:
	    buffer = gc_read_buffer(fd,buffer);

	    // Process telemetry packet:
	    gc_tlm_pkt_proc(buffer);
    } while (-1);

	return;
}