///////////////////////////////////////////////////////////////////////////////
//
// Initialize Camera
//
// Function responsible for preparing the camera, setting a custom
// exposure time, and restoring the camera to a normal state (without power
// cycling). It is called by command imaging task at task start as a function.
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
// - N/A
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

// Header files:
#include <config_cam_exp.h> // Configure camera exposure function declaration

int init_cam() {
    // Definitions and initializations:
    spinError errReturn = SPINNAKER_ERR_SUCCESS;
    spinError spin_ret_val = SPINNAKER_ERR_SUCCESS;

    spinSystem hSystem = NULL;
    spinCameraList hCameraList = NULL;

    spinCamera hCamera = NULL;

    spinNodeMapHandle hNodeMap = NULL;

    size_t numCameras = 0;

    unsigned int i = 0;

    // Retrieve singleton reference to system object:
    spin_ret_val = spinSystemGetInstance(&hSystem);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to retrieve system instance;"
            " aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Create camera list:
    spin_ret_val = spinCameraListCreateEmpty(&hCameraList);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to create camera list; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Retrieve list of cameras from the system:
    spin_ret_val = spinSystemGetCameras(hSystem,hCameraList);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to retrieve camera list; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Retrieve number of cameras:
    spin_ret_val = spinCameraListGetSize(hCameraList,&numCameras);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to retrieve number of cameras;"
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
            rt_printf("%d (INIT_CAM) Unable to clear camera list;"
                " aborting with error %d\n",time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }

        // Destroy camera list before releasing system:
        spin_ret_val = spinCameraListDestroy(hCameraList);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to destroy camera list; aborting"
                " with error %d\n",time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }

        // Release system
        spin_ret_val = spinSystemReleaseInstance(hSystem);\

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            rt_printf("%d (INIT_CAM) Unable to release system instance;"
                " aborting with error %d\n",time(NULL),spin_ret_val);

            // Exit:
            return spin_ret_val;
        }

        // Print:
        rt_printf("%d (INIT_CAM) Found %d cameras (expected 1); aborting\n",\
            time(NULL),numCameras);

        // Exit:
        return -1;
    }

    // Select camera:
    spin_ret_val = spinCameraListGet(hCameraList,0,&hCamera);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to retrieve camera from list;"
            " aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        errReturn = spin_ret_val;
    } else {
        // Initialize camera:
        spin_ret_val = spinCameraInit(hCamera);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to initialize camera; aborting"
                " with error %d\n",time(NULL),spin_ret_val);

            // Set return:
            errReturn = spin_ret_val;
        }

        // Retrieve GenICam nodemap:
        spin_ret_val = spinCameraGetNodeMap(hCamera,&hNodeMap);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to retrieve GenICam nodemap;"
                " aborting with error %d\n",time(NULL),spin_ret_val);
            
            // Set return:
            errReturn = spin_ret_val;
        }

        // Configure exposure:
        spin_ret_val = config_cam_exp(hNodeMap);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {

            // Set return:
            errReturn = spin_ret_val;
        }

        // De-initialize camera:
        spin_ret_val = spinCameraDeInit(hCamera);

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to de-initialize camera;"
                " non-fatal error %d\n",time(NULL),spin_ret_val);
            // Set return:
            errReturn = spin_ret_val;
        }

        // Check success:
        if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
            // Set return:
            errReturn = spin_ret_val;
        }
    }

    // Release camera:
    spin_ret_val = spinCameraRelease(hCamera);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to release camera; aborting with"
            " error %d\n",time(NULL),spin_ret_val);

        // Exit:
        errReturn = spin_ret_val;
    }

    // Clear camera list before releasing system:
    spin_ret_val = spinCameraListClear(hCameraList);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to clear camera list;"
            " aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Destroy camera list before releasing system:
    spin_ret_val = spinCameraListDestroy(hCameraList);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to destroy camera list; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Release system:
    spin_ret_val = spinSystemReleaseInstance(hSystem);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to release system instance; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Exit:
    return errReturn;
}