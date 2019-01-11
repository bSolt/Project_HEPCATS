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
#include <stdint.h>  // Stamdard integer types
#include "time.h"    // Standard time function definitions

#include "gc_telecmd_inputs_struct.h" // Structure definition

#include "get_doy.h" // Function definition

// Convert command string into command packet field inputs function
struct telecmd_pkt_inputs gc_interp_cmd_str(char* cmd_str_arr[])
{
    // Initialize:
    int i;

    // Define structure:
    struct telecmd_pkt_inputs telecmd_pkt_inputs;

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
    for (i = 0; i < 10; ++i){
    	// Check if command parameter flag is set:
	    if (strcmp("with",cmd_str_arr[i]) == 0) {
	    	// Get Application Data for command parameter:
	    	telecmd_pkt_inputs.pkt_app_dat_cmd_arg = 0; // PLACEHOLDER
	    }
	    // Command parameter flag is not set:
	    if (i == 9){
	    	// Set Application Data to idle:
	    	telecmd_pkt_inputs.pkt_app_dat_cmd_arg = 0xFF; // All '1's
	    }
	}

    // Check for command execution time:
    for (i = 0; i < 10; ++i){
    	// Check if execution time flag is set:
	    if (strcmp("hold",cmd_str_arr[i]) == 0){
			// Initialize:
			int cmd_time_str_ind = i + 1;

			// Split command execution time string:
		    char* p = strtok(cmd_str_arr[cmd_time_str_ind],"/-:"); // Split with /,-,:
		    char* time_str_arr[5]; int i = 0;

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

			// Set ATC flag:
			telecmd_pkt_inputs.pkt_app_dat_atc_flg = 1; // True

			// Exit loop:
			break;
		}
		// Execution time flag is not set:
	    if (i == 9){
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

			// Set ATC flag:
			telecmd_pkt_inputs.pkt_app_dat_atc_flg = 0; // False
	    }
	}

    // Create telecommand packet:
    return telecmd_pkt_inputs;
}
