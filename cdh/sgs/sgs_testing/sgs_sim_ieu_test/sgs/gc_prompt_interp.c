///////////////////////////////////////////////////////////////////////////////
//
// SGS and Simulated IEU Communication Test
// 
// Ground Control Prompt Interpreter
//
// This script would interpret the prompt string input typed in by the 
// operator. The different actions that an operator can do in Ground Control 
// is accomplished by specifying a specific "macros". These macros are 
// essentially instructions/commands for Ground Control. The interpreter looks 
// for which macro the user specifed and then starts the associated function to 
// execute that macro. 
//
// Macros defined right now:
//     - "cmd"
//	       - Send a command to an external element (I.E. hepcats)
// 
// -------------------------------------------------------------------------- /
//
// Input Arguments:
// - prompt_input_string (from prompt)
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
// Created: November 6, 2018
// 
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library 
#include <stdint.h>  // Integer types
#include <string.h>  // String function definitions 
#include <unistd.h>  // UNIX standard function definitions 
#include <fcntl.h>   // File control definitions 

#include "gc_macro_cmd.h" // Function definition

void main(int argc, char const *argv[])
{
	// Get input string:
	char input_str[50];
	strcpy(input_str,argv[1]);

	// Split input string to get macro:
    char* p = strtok(input_str," "); // Split with " "
    char* input_str_arr[10]; int i = 0;

    // Loop to split input string:
    while (p != NULL) {
        input_str_arr[i++] = p; 
        p = strtok(NULL," ");
    }

    // Check to see if macro is "cmd":
    if (strcmp("cmd",input_str_arr[0]) == 0){
		// Start command macro function:
    	gc_macro_cmd(input_str);
		
		// Exit:
		return;
    } 

    // If you get here, the macro is not recognized:
    printf("(GC_PROMPT_INTERP) <ERROR> \"%s\" macro not recognized\n",\
    	input_str_arr[0]);

    // Exit:
	return;
}