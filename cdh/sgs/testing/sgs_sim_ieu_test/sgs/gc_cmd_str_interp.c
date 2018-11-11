///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Ground Control Command String Interpretor
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

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <stdint.h>  // Integer types
#include "time.h"    // Standard time function definitions

#include "gc_telecmd_inputs_struct.h" // Structure definition

#include "get_doy.h" // Function definition

// Convert command string into command packet field inputs function
struct telecmd_pkt_inputs gc_cmd_str_interp(char* cmd_str)
{
	// Define structure:
	struct telecmd_pkt_inputs telecmd_pkt_inputs;

	// Split command string intho array
    char* p = strtok(cmd_str," "); // Split with " "
    char* cmd_str_arr[7]; int i = 0;

    // Loop to split command string:
    while (p != NULL) {
        cmd_str_arr[i++] = p; 
        p = strtok(NULL," ");
    }

    // Fill in rest of command string array (to prevent segmentation faults):
    for (int j = i; j < 7; ++j) {
    	cmd_str_arr[j] = "empty";
    }

    // Check for correct external element "hepcat":
    if (strcmp("hepcats",cmd_str_arr[1]) == 1) {		
    	// Print error message
		printf("(GC_CMD_STR_INTERP) <ERROR> \"%s\" external element"
			" not recognized\n",cmd_str_arr[1]);

		// Exit:
    	exit(0);
    }

    // Get APID and Packet Name for command mnemonic:
    // HARDCODED!! WILL HAVE TO MAKE DATABASE FOR THE FUTURE!
    telecmd_pkt_inputs.pkt_apid = 0; // PLACEHOLDER
    telecmd_pkt_inputs.pkt_name = 0; // PLACEHOLDER

    // Check for command parameter:
    if (strcmp("with",cmd_str_arr[3]) == 0) {
    	// Get Application Data for command parameter:
    	// HARDCODED!! WILL HAVE TO MAKE DATABASE FOR THE FUTURE!
    	telecmd_pkt_inputs.pkt_app_data = 0; // PLACEHOLDER
    } else {
    	// Set Application Data to idle:
    	telecmd_pkt_inputs.pkt_app_data = 4294967295; // All '1's 
    }

    // Check for command execution time:
    if ((strcmp("hold",cmd_str_arr[3]) == 0) || \
    	(strcmp("hold",cmd_str_arr[5]) == 0)) {
		// Initialize:
		int cmd_time_str_ind;

		// Determine where the command execution time string is located:
		if (strcmp("empty",cmd_str_arr[5]) == 0){
			cmd_time_str_ind = 4;
		} else {
			cmd_time_str_ind = 6;
		}

		// Split command execution time string:
	    p = strtok(cmd_str_arr[cmd_time_str_ind],"/-:"); // Split with /,-,:
	    char* time_str_arr[5]; i = 0;

		// Loop to split time string:
		while (p != NULL) {
		    time_str_arr[i++] = p;
		    p = strtok(NULL,"/-:");
		}

		// Save time:
		telecmd_pkt_inputs.pkt_t_year = atoi(time_str_arr[0]);
		telecmd_pkt_inputs.pkt_t_doy = atoi(time_str_arr[1]);
		telecmd_pkt_inputs.pkt_t_hour = atoi(time_str_arr[2]);
		telecmd_pkt_inputs.pkt_t_min = atoi(time_str_arr[3]);
		telecmd_pkt_inputs.pkt_t_sec = atoi(time_str_arr[4]);
    } else {
		// Define time variables and get current time in UTC:
		struct tm *tm; time_t now;
		time(&now); tm = gmtime(&now);

	    // Save time:
		telecmd_pkt_inputs.pkt_t_year = tm->tm_year+1900;
		telecmd_pkt_inputs.pkt_t_doy = get_doy(tm->tm_mon+1,\
			tm->tm_mday,tm->tm_year+1900);
		telecmd_pkt_inputs.pkt_t_hour = tm->tm_hour;
		telecmd_pkt_inputs.pkt_t_min = tm->tm_min;
		telecmd_pkt_inputs.pkt_t_sec = tm->tm_sec;
    }

    // Create telecommand packet:
    return telecmd_pkt_inputs;
}