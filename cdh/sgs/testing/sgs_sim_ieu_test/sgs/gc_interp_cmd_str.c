///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
//
// Convert command string into command packet field inputs function
//
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - cmd_str
//
// Output Arguments:
// - telecmd_pkt_inputs
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

// Macro definitions:
#define __USE_XOPEN // Required for strptime()
#define _GNU_SOURCE // Required for strptime()

// Standard libraries
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions
#include <stdint.h>  // Integer types
#include <time.h>    // Standard time function definitions

// Header files:
#include "gc_telecmd_inputs_struct.h" // Structure declaration

struct telecmd_pkt_inputs gc_interp_cmd_str(char* cmd_str_arr[]) {
    // Define structures:
    struct telecmd_pkt_inputs \
        telecmd_pkt_inputs; // Telecommand inputs structure
    struct tm tm;           // Time structure
    struct timespec spec;   // Time structure
    time_t sec;             // Unix timestamp

    // Definitions:
    int cmd_time_str_ind;

    // Check for correct external element "hepcat":
    if (strcmp("hepcats",cmd_str_arr[1]) == 1) {        
        // Print error message
        printf("(GC_CMD_STR_INTERP) <ERROR> \"%s\" external element"
            " not recognized\n",cmd_str_arr[1]);

        // Exit:
        exit(0);
    }

    // Get APID and Packet Name for command mnemonic:
    telecmd_pkt_inputs.pkt_apid = 0; // PLACEHOLDER
    telecmd_pkt_inputs.pkt_name = 0; // PLACEHOLDER

    // Check for command parameter:
    for (int i = 0; i < 10; ++i) {
        // Check if command parameter flag is set:
        if (strcmp("with",cmd_str_arr[i]) == 0) {
            // Get Application Data for command parameter:
            telecmd_pkt_inputs.pkt_app_dat_cmd_arg = 0; // PLACEHOLDER
        }
        // Command parameter flag is not set:
        if (i == 9) {
            // Set Application Data to idle:
            telecmd_pkt_inputs.pkt_app_dat_cmd_arg = 0xFF; // All '1's 
        }
    }

    // Check for command execution time:
    for (int i = 0; i < 10; ++i) {
        // Check if execution time flag is set:
        if (strcmp("hold",cmd_str_arr[i]) == 0) {
            // Set index where command execution time is located
            // in command string array:
            cmd_time_str_ind = i + 1;

            // Convert command execution string into the (numerical) 
            // time structure:
            strptime(cmd_str_arr[cmd_time_str_ind],"%Y/%j-%H:%M:%S",&tm);

            // Convert time structure into UNIX time stamp:
            sec = mktime(&tm);

            // Set command execution time:
            telecmd_pkt_inputs.pkt_sec_hdr_t_sec  = sec;
            telecmd_pkt_inputs.pkt_sec_hdr_t_msec = 0;
            telecmd_pkt_inputs.pkt_sec_hdr_t_void = 0xFF;

            // Set ATC flag:
            telecmd_pkt_inputs.pkt_app_dat_atc_flg = 1; // True

            // Exit loop:
            break;
        }
        // Execution time flag is not set:
        if (i == 9) {
            // Get current UNIX time stamp: 
            clock_gettime(CLOCK_REALTIME, &spec);

            // Set command execution time:
            telecmd_pkt_inputs.pkt_sec_hdr_t_sec  = spec.tv_sec;
            telecmd_pkt_inputs.pkt_sec_hdr_t_msec =  0;
            telecmd_pkt_inputs.pkt_sec_hdr_t_void = 0xFF;

            // Set ATC flag:
            telecmd_pkt_inputs.pkt_app_dat_atc_flg = 0; // False
        }
    }

    // Return:
    return telecmd_pkt_inputs;
}