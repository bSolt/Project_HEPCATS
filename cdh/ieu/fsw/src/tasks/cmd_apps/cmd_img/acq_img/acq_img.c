///////////////////////////////////////////////////////////////////////////////
//
// Acquire Image
//
// Function responsible for opening and initializing the camera for use. Once
// the camera is ready, run camera single function is called to aquire an
// image.
//
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - Xenomai / Cobalt
// - Spinnaker
//
// Input Arguments:
// - N/A
//
// Output Arguments:
// - spin_ret_val
//
// -------------------------------------------------------------------------- /
//
// Christopher Percy
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

// Header files:
#include <run_cam_sgl.h> // Acquire image from camera function declaration

int acq_img() {
    // Print:
    rt_printf("%d (ACQ_IMG) Initializing camera\n",time(NULL));

    // Definitions and initializations:
    spinError spin_ret_val = SPINNAKER_ERR_SUCCESS;

    spinCamera hCamera = NULL; // Camera object
    spinSystem hSystem = NULL; // System object
    
    spinCameraList hCameraList = NULL; // Camera list

    spinNodeMapHandle hNodeMapTLDevice = NULL; // TL node map

    spinNodeMapHandle hNodeMap = NULL; // Node map

    spinNodeHandle hAcquisitionMode = NULL;
    spinNodeHandle hAcquisitionModeSingle = NULL;
    int64_t acquisitionModeSingle = 0;

    size_t numCameras = 0;

    // Retrieve singleton reference to system object:
    spin_ret_val = spinSystemGetInstance(&hSystem);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to retrieve system instance;"
            " aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Create camera list:
    spin_ret_val = spinCameraListCreateEmpty(&hCameraList);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to create camera list; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Retrieve list of cameras from the system:
    spin_ret_val = spinSystemGetCameras(hSystem,hCameraList);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to retrieve camera list; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Retrieve number of cameras:
    spin_ret_val = spinCameraListGetSize(hCameraList,&numCameras);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to retrieve number of cameras;"
            " aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Check number of cameras:
    // (Only one camera is expected)
    if (numCameras != 1) {
        // Clear camera list before releasing system:
        spin_ret_val = spinCameraListClear(hCameraList);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (ACQ_IMG) Unable to clear camera list;"
                " aborting with error %d\n",time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }

        // Destroy camera list before releasing system:
        spin_ret_val = spinCameraListDestroy(hCameraList);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (ACQ_IMG) Unable to destroy camera list; aborting"
                " with error %d\n",time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }

        // Release system
        spin_ret_val = spinSystemReleaseInstance(hSystem);\

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            rt_printf("%d (ACQ_IMG) Unable to release system instance;"
                " aborting with error %d\n",time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }

        // Print:
        rt_printf("%d (ACQ_IMG) Found %d cameras (expected 1); aborting"
            " acquisition\n",time(NULL),numCameras);

        // Exit:
        return -1;
    }

    // Select camera:
    spin_ret_val = spinCameraListGet(hCameraList,0,&hCamera);

    // Get TL node maps:
    spin_ret_val = spinCameraGetTLDeviceNodeMap(hCamera,&hNodeMapTLDevice);

    // Initialize camera:
    spin_ret_val = spinCameraInit(hCamera);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to initialize camera; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Retrieve GenICam nodemap:
    spin_ret_val = spinCameraGetNodeMap(hCamera,&hNodeMap);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to retrieve GenICam nodemap;"
            " aborting with error %d\n",time(NULL),spin_ret_val);
        
        // Exit:
        return spin_ret_val;
    }

    // Get acquisition mode node map:
    spin_ret_val = spinNodeMapGetNode(hNodeMap,"AcquisitionMode",\
        &hAcquisitionMode);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to get aquisition mode"
            " node map; aborting with error %d\n",time(NULL),\
            spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Set single frame acquisition mode:
    spin_ret_val = spinEnumerationGetEntryByName(hAcquisitionMode,\
        "SingleFrame",&hAcquisitionModeSingle);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to set aquisition mode"
            " to single frame; aborting with error %d\n",time(NULL),\
            spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Set single frame acquisition mode:
    spin_ret_val = spinEnumerationEntryGetIntValue(hAcquisitionModeSingle,\
        &acquisitionModeSingle);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to set acquisition mode"
            " to single frame; aborting with error %d\n",time(NULL),\
            spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Set the integer as the new enumeration value:
    spin_ret_val = spinEnumerationSetIntValue(hAcquisitionMode,\
        acquisitionModeSingle);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to set acquisition mode"
            " to single frame; aborting with error %d\n",time(NULL),\
            spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Aquire image:
    spin_ret_val = run_cam_sgl(hCamera,hNodeMap,hNodeMapTLDevice);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to acquire image;"
            " aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // De-initialize camera:
    spin_ret_val = spinCameraDeInit(hCamera);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        rt_printf("%d (ACQ_IMG) Unable to de-initialize camera;"
            " non-fatal error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Release camera:
    spin_ret_val = spinCameraRelease(hCamera);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to release camera; aborting with"
            " error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Clear camera list before releasing system:
    spin_ret_val = spinCameraListClear(hCameraList);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to clear camera list;"
            " aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Destroy camera list before releasing system:
    spin_ret_val = spinCameraListDestroy(hCameraList);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to destroy camera list; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Release system:
    spin_ret_val = spinSystemReleaseInstance(hSystem);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (ACQ_IMG) Unable to release system instance; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    return spin_ret_val;
}