///////////////////////////////////////////////////////////////////////////////
//
// Send MDQ (Magnetometer DAQ) Command
//
// Function responsible for sending a command to the DAQ using libusb. A 
// command string and size is passed to this function. The command string is
// assumed to be valid. If it is not valid, the function will return with
// error.
//
// All commands echo if the DAQ is not scanning. Commands will not echo while
// scanning is active to prevent interruption of the data stream. In this
// sense, the "start" command never echoes, and the stop command always
// echoes. Any valid command passed to this function except for the "start"
// command expects a response echoing the command and an additional
// response if applicable. If the expected response is received from the DAQ,
// the command was successful.
//
// DAQ protocol communication protocol white paper:
// https://www.dataq.com/resources/pdfs/misc/DI-4108-DI-4208-Protocol.pdf
// 
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - Xenomai 3 / Cobalt
// - libusb-1.0
//
// Input Arguments:
// - cmd_str
// - cmd_size
//
// Output Arguments:
// - ret_val
//
// -------------------------------------------------------------------------- /
//
// Benjamin Spencer
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: February 24, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdlib.h>  // Standard library
#include <stdio.h>   // Standard input output library
#include <unistd.h>  // UNIX standard function definitions
#include <errno.h>   // Error number definitions
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types

// Xenomai libraries:
#include <alchemy/task.h>  // Task management service

// External libraries:
#include <libusb-1.0/libusb.h> // Library to access USB devices

// Header files:
#include <mdq_dev.h> // Magnetometer DAQ device variable declarations

// Variable definitions:
libusb_device_handle* dev_hdl; // Device handle
libusb_context* ctx;           // libusb session

int8_t send_mdq_cmd(char* cmd_str) {
    // Definitions and initializations:
    int8_t ret_val = 0; // Function return value

    uint8_t rsp_exp_flg; // Command response expected flag

    char cmd[50];          // Command string buffer
    char rsp_str[200];     // Response string buffer
    char exp_rsp_str[200]; // Expected response string buffer

    uint8_t bytes = 0; // Bytes written or received

    // Print:
    rt_printf("%d (SEND_MDQ_CMD) Received command; sending \"%s\"\n",\
        time(NULL),cmd_str);

    // Append return carriage to command:
    sprintf((char*) cmd,"%s\r",cmd_str);

    // Create expected response by checking if input command matches a
    // command whose response includes a response in addition to echoing
    // the command. If the command does not match these known commands, the
    // expected response is assumed to an echo of the command. If the command
    // is "start 0" (start scanning), then no response is expected so the
    // check is skipped.
    if (strcmp(cmd,"info 0\r") == 0) {
        // Set expected response:
        strcpy(exp_rsp_str,"info 0 DATAQ\r");
    } else if (strcmp(cmd,"info 1\r") == 0) {
        // Set expected response:
        strcpy(exp_rsp_str,"info 1 4108\r");
    } else if (strcmp(cmd,"info 2\r") == 0) {
        // Set expected response:
        strcpy(exp_rsp_str,"info 2 75\r");
    } else if (strcmp(cmd,"info 6\r") == 0) {
        // Set expected response:
        strcpy(exp_rsp_str,"info 6 5C0F73C6\r");
    } else if (strcmp(cmd,"info 9\r") == 0) {
        // Set expected response:
        strcpy(exp_rsp_str,"info 9 60000000\r"); 
    } else if (strcmp(cmd,"start 0\r") == 0) {
        // Set expected response:
        strcpy(exp_rsp_str,"start 0\r"); 
    } else {
        // Set expected response:
        strcpy(exp_rsp_str,cmd);
    }

    // Send command:
    ret_val = libusb_bulk_transfer(dev_hdl,(1|LIBUSB_ENDPOINT_OUT),\
        (unsigned char*) cmd,sizeof(cmd),&bytes,0); 

    // See if response is expected or not:
    // (For some reason, this is the only way I can set the flag)
    if (strcmp(exp_rsp_str,"start 0\r") == 0) { 
        // Set expect response flag:
        rsp_exp_flg = 0; // No response expected
    } else {
        // Set expect response flag:
        rsp_exp_flg = 1; // Response expected
    }

    // Check success:
    // (If no return error, number of bytes written is expected, and
    // command response is expected)
    if ((ret_val == 0) && (bytes == sizeof(cmd)) && (rsp_exp_flg == 1)) {
        // Receive response:
        ret_val = libusb_bulk_transfer(dev_hdl,(1|LIBUSB_ENDPOINT_IN),\
            (unsigned char*) rsp_str,sizeof(rsp_str),&bytes,0);

        // Check success and if response is expected flag:
        if (ret_val == 0) {
            // Compare response to expected response:
            if ((strcmp(rsp_str,exp_rsp_str) == 0) || \
                (strcmp(rsp_str,exp_rsp_str) > 0)) {
                // Print:
                rt_printf("%d (SEND_MDQ_CMD) Received expected command"
                    " response; command executed successfully\n",time(NULL));
            } else {
                // Print:
                rt_printf("%d (SEND_MDQ_CMD) Did not receive expected command"
                    " response; command did not execute\n",time(NULL));

                // Set return value:
                ret_val = -1; 
            }
        } else {
            // Print:
            rt_printf("%d (SEND_MDQ_CMD) Error reading command response"
                "; aborting with error %d\n",time(NULL),ret_val);

            // Exit:
            return ret_val;
        }
    } else if (rsp_exp_flg == 0) {
        // Print:
        rt_printf("%d (SEND_MDQ_CMD) Command response not expected;"
            " ignoring check\n",time(NULL));
    } else if (ret_val < 0) {
        // Print:
        rt_printf("%d (SEND_MDQ_CMD) Error sending command; aborting"
            " with error %d\n",time(NULL),ret_val);
    }

    // Return:
    return ret_val;
}