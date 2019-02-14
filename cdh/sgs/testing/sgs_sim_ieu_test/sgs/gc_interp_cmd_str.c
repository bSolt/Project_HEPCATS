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

// Macro definitions:
#define MAX_CMDS 100 // Max number of commands in database

struct telecmd_pkt_inputs gc_interp_cmd_str(char* cmd_str_arr[]) {
   // Define structures:
    struct telecmd_pkt_inputs \
        telecmd_pkt_inputs; // Telecommand inputs structure
    struct tm tm;           // Time structure

    // Definitions and initializations:
    int i;
    int j;
    int row;

    char cmd_mnem[MAX_CMDS][20];    // Command mnemonic database
    int  cmd_apid[MAX_CMDS];        // Command APID database
    int  cmd_pkt_nm[MAX_CMDS];      // Command packet name database
    char prm_mnem[MAX_CMDS][6][20]; // Command parameter mnemonic database
    int  cmd_arg[MAX_CMDS][6];      // Command argument database

    char line[50]; // Line from file
    FILE* fd; // File descriptor

    int cmd_time_str_ind; // Location of execution time

    // Open command database:
    fd = fopen("command_database.csv", "r");

    // Read database line by line:
    while(fgets(line, sizeof(line), fd) != NULL) {
        // Strip newline character:
        line[strcspn(line,"\n")] = 0;

        // Parse by comma:
        char* token = strtok(line, ",");

        // Parse by comma:
        while (token != NULL) {
            // Switch case to split line into... 
            switch (i) {
                // Command mnemonic (string)
                case 0 :
                    // Copy string:
                    strcpy(cmd_mnem[j],token);

                    // Exit:
                    break;
                // Command APID (integer):
                case 1 :
                    // Scan to get APID:
                    sscanf(token,"%x",&cmd_apid[j]);

                    // Exit:
                    break;
                // Command packet name (integer):
                case 2 :
                    // Scan to packet name:
                    sscanf(token,"%x",&cmd_pkt_nm[j]);
                    break;
                // Command parameter mnemonic (string):
                case 3 :
                    // Copy string:
                    strcpy(prm_mnem[j][0],token);

                    // Exit:
                    break;
                // Command argument (integer):
                case 4 :
                    // Scan to get argument:
                    sscanf(token,"%x",&cmd_arg[j][0]);

                    // Exit:
                    break;
                // Command parameter mnemonic (string):
                case 5 :
                    // Copy string:
                    strcpy(prm_mnem[j][1],token);

                    // Exit:
                    break;
                // Command argument (integer):
                case 6 :
                    // Scan to get argument:
                    sscanf(token,"%x",&cmd_arg[j][1]);

                    // Exit:
                    break;
                // Command parameter mnemonic (string):
                case 7 :
                    // Copy string:
                    strcpy(prm_mnem[j][2],token);

                    // Exit:
                    break;
                // Command argument (integer):
                case 8 :
                    // Scan to get argument:
                    sscanf(token,"%x",&cmd_arg[j][2]);

                    // Exit:
                    break;
                // Command parameter mnemonic (string):
                case 9 :
                    // Copy string:
                    strcpy(prm_mnem[j][3],token);

                    // Exit:
                    break;
                // Command argument (integer):
                case 10 :
                    // Copy string:
                    sscanf(token,"%x",&cmd_arg[j][3]);

                    // Exit:
                    break;
                // Command parameter mnemonic (string):
                case 11 :
                    // Copy string:
                    strcpy(prm_mnem[j][4],token);

                    // Exit:
                    break;
                // Command argument (integer):
                case 12 :
                    // Copy string:
                    sscanf(token,"%x",&cmd_arg[j][4]);

                    // Exit:
                    break;
                // Command parameter mnemonic (string):
                case 13 :
                    strcpy(prm_mnem[j][5],token);

                    // Exit:
                    break;
                // Command argument (integer):
                case 14 :
                    // Copy string:
                    sscanf(token,"%x",&cmd_arg[j][5]);

                    // Exit:
                    break;
            }
            // Increment counter:
            i++;

            // Parse by comma:
            token = strtok(NULL, ",");
        }

        // Increment counter:
        j++;

        // Reset counter:
        i = 0;
    }

    // Close file:
    fclose(fd);

    // Check for correct external element "hepcat":
    if (strcmp("hepcats",cmd_str_arr[1]) == 1) {        
        // Print error message
        printf("(GC_CMD_STR_INTERP) <ERROR> \"%s\" external element"
            " not recognized\n",cmd_str_arr[1]);

        // Exit:
        exit(0);
    }

    // Get APID and Packet Name for command mnemonic:
    for (int i = 0; i < MAX_CMDS; ++i) {
        // Loop through database to match input command mnemonic:
        if (strcmp(cmd_mnem[i],cmd_str_arr[2]) == 0) {
            // Set APID and packet name:
            telecmd_pkt_inputs.pkt_apid = cmd_apid[i];
            telecmd_pkt_inputs.pkt_name = cmd_pkt_nm[i];

            // Save row
            row = i;

            // Exit:
            break; 
        }

        // Input command mnemonic does not match what is in database:
        if (i == (MAX_CMDS - 1)) {
            // Print error message
            printf("(GC_CMD_STR_INTERP) <ERROR> \"%s\" command mnemonic"
            " not recognized\n",cmd_str_arr[2]);

            // Exit:
            exit(0);
        }
    }

    // Set Application Data to idle:
    // (will be replaced if command parameter is inputted)
    telecmd_pkt_inputs.pkt_app_dat_cmd_arg = 0x7FFFFFFF; // "1"s 

    // Check for command parameter:
    for (i = 0; i < 10; ++i) {
        // Check if command parameter flag is set:
        if (strcmp("with",cmd_str_arr[i]) == 0) {
            for (j = 0; j < 6; ++j) {
                if (strcmp(prm_mnem[row][j],cmd_str_arr[i+1]) == 0) {
                    // Get Application Data for command parameter:
                    telecmd_pkt_inputs.pkt_app_dat_cmd_arg = cmd_arg[row][j];

                    // Exit:
                    break;
                }

                // Command parameter does not match what is in database:
                if (j == 5) {
                    // Print error message
                    printf("(GC_CMD_STR_INTERP) <ERROR> \"%s\" command"
                    " parameter not recognized\n",cmd_str_arr[i+1]);

                    // Exit:
                    exit(0);
                }
            }
        }
    }

    // Check for command execution time:
    for (i = 0; i < 10; ++i) {
        // Check if execution time flag is set:
        if (strcmp("hold",cmd_str_arr[i]) == 0) {
            // Set environment to UTC:
            // (required for mktime)
            setenv("TZ", "UTC", 1);

            // Set index where command execution time is located
            // in command string array:
            cmd_time_str_ind = i + 1;

            // Convert command execution string into a time structure:
            // (string format: YYYY/DOY-HH:MM:SS)
            strptime(cmd_str_arr[cmd_time_str_ind],"%Y/%j-%H:%M:%S",&tm);

            // Set command execution time to Unix timestamp of command string:
            telecmd_pkt_inputs.pkt_sec_hdr_t_sec  = mktime(&tm);
            telecmd_pkt_inputs.pkt_sec_hdr_t_msec = 0;
            telecmd_pkt_inputs.pkt_sec_hdr_t_void = 0xFF;

            // Set ATC flag:
            telecmd_pkt_inputs.pkt_app_dat_atc_flg = 1; // True

            // Exit loop:
            break;
        }
        // Execution time flag is not set:
        if (i == 9) {
            // Set command execution time to current Unix timestamp:
            telecmd_pkt_inputs.pkt_sec_hdr_t_sec  = time(NULL);
            telecmd_pkt_inputs.pkt_sec_hdr_t_msec = 0;
            telecmd_pkt_inputs.pkt_sec_hdr_t_void = 0xFF;

            // Set ATC flag:
            telecmd_pkt_inputs.pkt_app_dat_atc_flg = 0; // False
        }
    }

    // Return:
    return telecmd_pkt_inputs;
}
