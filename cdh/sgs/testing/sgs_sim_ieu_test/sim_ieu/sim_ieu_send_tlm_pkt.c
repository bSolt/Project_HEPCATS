///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Simulated IEU Receive Ground TeleCommand
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
#include <stdint.h>  // Integer types
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <fcntl.h>   // File control definitions
#include <errno.h>   // Error number definitions
#include <termios.h> // POSIX terminal control definitions

// Header files:
#include "sim_ieu_open_port.h"    // Open port function declaration
#include "sim_ieu_crt_tlm_pkt.h"  // Create packet function declaraton
#include "sim_ieu_write_port.h" // Write buffer function declaration

void main(int argc, char const *argv[]) {
    // Create telemetry packet:
    char* buffer = malloc(1080*sizeof(char));
    buffer = sim_ieu_crt_tlm_pkt(buffer);

    // Open port:
    int fd = sim_ieu_open_port("/dev/pts/2");

    // Write buffer to port:
    sim_ieu_write_port(fd,buffer);

    // Close port:
    close(fd);

    return;
}