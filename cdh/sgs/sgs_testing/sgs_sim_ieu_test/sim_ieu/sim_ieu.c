///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Simulated IEU Receive Ground Command
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
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Integer types

#include "sim_ieu_open_port.h"        // Function definition
#include "sim_ieu_read_buffer.h"      // Function definition
#include "sim_ieu_telecmd_pkt_proc.h" // Function definition

void main(int argc, char const *argv[])
{
	// Open port:
	int fd = sim_ieu_open_port("/dev/pts/3");

	// Read from buffer:
	char* buffer = malloc(20*sizeof(char));
	buffer = sim_ieu_read_buffer(fd,buffer);

	// Process telecommand packet:
	sim_ieu_telecmd_pkt_proc(buffer);

	// Close port:
	close(fd);

	return;
}