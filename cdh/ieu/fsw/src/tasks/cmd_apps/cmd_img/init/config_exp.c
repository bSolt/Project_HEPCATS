///////////////////////////////////////////////////////////////////////////////
//
// Configure Camera Exposure
//
// Function responsible for setting the camera's exposure. It is called by
// initialize camera function.
//
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - Xenomai / Cobalt
// - Spinnaker
//
// Input Arguments:
// - hNodeMap
//
// Output Arguments:
// - ret_val
//
// -------------------------------------------------------------------------- /
//
// FLIR Integrated Imaging Solutions, Inc
// ASEN 4018
// Project HEPCATS
// Subsystem: C&DH
// Created: February 13, 2019
//
///////////////////////////////////////////////////////////////////////////////

// Standard libraries:
#include <stdlib.h>  // Standard library
#include <stdio.h>   // Standard input/output definitions
#include <unistd.h>  // UNIX standard function definitions
#include <errno.h>   // Error number definitions
#include <string.h>  // String function definitions
#include <stdint.h>  // Standard integer types
#include <time.h>    // Standard time types

// Xenomai libraries:
#include <alchemy/task.h> // Task management service

// Spinnaker libraries:
#include <SpinnakerC.h> // FLIR machine vision camera interface definitions

// Macro definitions:
#define CAM_EXPOSURE 200000; // Camera exposure in microseconds

// Check if a node is available and readable:
bool8_t IsAvailableAndReadable(spinNodeHandle hNode, char nodeName[]) {
    // Definitions and initializations:
    spinError spin_ret_val = SPINNAKER_ERR_SUCCESS; // Spinnaker function
                                                    // return value

    bool8_t pbAvailable = False;
    bool8_t pbReadable = False;

    // Check if node is available:
    spin_ret_val = spinNodeIsAvailable(hNode, &pbAvailable);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (CONFIG_CAM_EXP) Unable to retrieve node availability"
            " (%s node) with error %d\n",time(NULL),nodeName,spin_ret_val);
    }

    // Check if node is readable:
    spin_ret_val = spinNodeIsReadable(hNode, &pbReadable);
    
    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (CONFIG_CAM_EXP) Unable to retrieve node readability"
            " (%s node) with error %d\n",time(NULL),nodeName,spin_ret_val);
    }

    // Exit:
    return pbReadable && pbAvailable;
}

// Check if a node is available and writable:
bool8_t IsAvailableAndWritable(spinNodeHandle hNode, char nodeName[]) {
    // Definitions and initializations:
    spinError spin_ret_val = SPINNAKER_ERR_SUCCESS; // Spinnaker function
                                                    // return value

    bool8_t pbAvailable = False;
    bool8_t pbWritable = False;

    // Check if node is available:
    spin_ret_val = spinNodeIsAvailable(hNode, &pbAvailable);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (CONFIG_CAM_EXP) Unable to retrieve node availability"
            " (%s node) with error %d\n",time(NULL),nodeName,spin_ret_val);
    }

    // Check if node is writable:
    spin_ret_val = spinNodeIsWritable(hNode, &pbWritable);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (CONFIG_CAM_EXP) Unable to retrieve node writ-ability"
            " (%s node) with error %d\n",time(NULL),nodeName,spin_ret_val);
    }

    // Exit:
    return pbWritable && pbAvailable;
}

// Print information when a node or entry is unavailable or not
// readable/writable on the connected camera:
void PrintRetrieveNodeFailure(char node[], char name[]) {
    // Print:
    rt_printf("%d (CONFIG_CAM_EXP) Unable to get %s (%s %s retrieval"
        " failed)\n",time(NULL),node, name, node);
}

spinError config_cam_exp(spinNodeMapHandle hNodeMap) {
    // Print:
    rt_printf("%d (CONFIG_CAM_EXP) Configuring camera exposure\n",time(NULL));

    // Definitions and initializations:
    spinError spin_ret_val = SPINNAKER_ERR_SUCCESS; // Spinnaker function
                                                    // return value

    int64_t exposureAutoOff;

    spinNodeHandle hExposureAutoOff;
    spinNodeHandle hExposureAuto;

    spinNodeHandle hExposureTime;
    double exposureTimeMax;

    double exposureTimeToSet;

    // Turn off automatic exposure mode:
    // (Automatic exposure prevents the manual configuration of exposure time
    // and needs to be turned off)
    hExposureAuto = NULL;
    hExposureAutoOff = NULL;
    exposureAutoOff = 0;

    // Retrieve enumeration node from nodemap:
    spin_ret_val = spinNodeMapGetNode(hNodeMap,"ExposureAuto",&hExposureAuto);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (CONFIG_CAM_EXP) Unable to disable automatic exposure"
        " (node retrieval); aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Retrieve entry node from enumeration node:
    // (If camera is available and readable)
    if (IsAvailableAndReadable(hExposureAuto,"ExposureAuto")) {
        spin_ret_val = spinEnumerationGetEntryByName(hExposureAuto,"Off",\
            &hExposureAutoOff);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (CONFIG_CAM_EXP Unable to disable automatic"
                " exposure (enum entry retrieval); aborting with error %d\n",\
                time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }
    } else {
        // Print node failure:
        PrintRetrieveNodeFailure("node","ExposureAuto");

        // Exit:
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Retrieve integer value from entry node:
    // (If camera is available and readable)
    if (IsAvailableAndReadable(hExposureAutoOff,"ExposureAutoOff")) {
        spin_ret_val = spinEnumerationEntryGetIntValue(hExposureAutoOff,\
            &exposureAutoOff);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (CONFIG_CAM_EXP) Unable to disable automatic"
            " exposure (enum entry int value retrieval); aborting with error"\
                " %d\n",time(NULL),spin_ret_val);

            // Exit
            return spin_ret_val;
        }
    } else {
        // Print node failure:
        PrintRetrieveNodeFailure("entry","ExposureAuto 'Off'");

        // Exit:
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Set integer as new value for enumeration node (disable automatic
    // exposure):
    // (If camera is available and readable)
    if (IsAvailableAndWritable(hExposureAuto,"ExposureAuto")) {
        spin_ret_val = spinEnumerationSetIntValue(hExposureAuto,\
            exposureAutoOff);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (CONFIG_CAM_EXP) Unable to disable automatic"
                " exposure (enum entry setting). Aborting with error %d\n",\
                time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }
    } else {
        // Print node failure:
        PrintRetrieveNodeFailure("node","ExposureAuto");

        // Exit:
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Set exposure time manually:
    hExposureTime     = NULL;
    exposureTimeMax   = 0.0;          // Microseconds
    exposureTimeToSet = CAM_EXPOSURE; // Microseconds

    // Retrieve exposure time node:
    spin_ret_val = spinNodeMapGetNode(hNodeMap,"ExposureTime",&hExposureTime);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (CONFIG_CAM_EXP) Unable to set exposure time; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Retrieve maximum value:
    // (If camera is available and readable)
    if (IsAvailableAndReadable(hExposureTime, "ExposureTime")) {
        spin_ret_val = spinFloatGetMax(hExposureTime, &exposureTimeMax);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (CONFIG_CAM_EXP) Unable to set exposure time;"
                " aborting with error %d\n",time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }
    } else {
        // Print node failure:
        PrintRetrieveNodeFailure("node", "ExposureTime");

        // Exit:
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Check if desired exposure time exceeds maximum:
    if (exposureTimeToSet > exposureTimeMax) {
        // Set exposure time to max:
        exposureTimeToSet = exposureTimeMax;
    }

    // Set desired exposure time as new value:
    // (If camera is available and readable)
    if (IsAvailableAndWritable(hExposureTime,"ExposureTime")) {
        spin_ret_val = spinFloatSetValue(hExposureTime, exposureTimeToSet);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (CONFIG_CAM_EXP) Unable to set exposure time;"
                " aborting with error %d\n",time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }
    } else {
        // Print node failure:
        PrintRetrieveNodeFailure("node","ExposureTime");

        // Exit:
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Print:
    rt_printf("%d (CONFIG_CAM_EXP) Camera exposure set to %f us\n",time(NULL),\
        exposureTimeToSet);

    // Exit:
    return spin_ret_val;
}