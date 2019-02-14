///////////////////////////////////////////////////////////////////////////////
//
// Interpret Prompt
// 
// Interprets prompt input
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

// Standard libraries: 
#include <stdio.h>   // Standard input/output definitions
#include <stdlib.h>  // Standard library
#include <stdint.h>  // Integer types
#include <string.h>  // String function definitions
#include <unistd.h>  // UNIX standard function definitions

// Header files:
#include "macro_cmd.h" // Command macro function declaration

void main(int argc, char const *argv[]) {
    // Definitions and initializations:
    char* input_str_arr[10] = \
        {" "," "," "," "," "," "," "," "," "," "};

    // Loop to read in prompt string input:
    for (int i = 0; i < 10; ++i){
        if (argv[i+1])
            input_str_arr[i] = strdup(argv[i+1]);
    }
    
    // Check to see if macro is "cmd":
    if (strcmp("cmd",input_str_arr[0]) == 0){
        // Start command macro function:
        macro_cmd(input_str_arr);
        
        // Exit:
        return;
    } 

    // If you get here, the macro is not recognized:
    printf("(PROMPT_INTERP) <ERROR> \"%s\" macro not recognized\n",\
        input_str_arr[0]);

    // Exit:
    return;
}