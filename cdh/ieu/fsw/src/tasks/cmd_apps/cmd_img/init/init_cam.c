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
    spinError err_ret = SPINNAKER_ERR_SUCCESS;
    spinError ret_val = SPINNAKER_ERR_SUCCESS;

    spinSystem hSystem = NULL;
    spinCameraList hCameraList = NULL;

    spinCamera hCamera = NULL;

    spinNodeMapHandle hNodeMap = NULL;

    size_t numCameras = 0;

    // Retrieve singleton reference to system object:
    ret_val = spinSystemGetInstance(&hSystem);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to retrieve system instance;"
            " aborting with error %d\n",time(NULL),ret_val);

        // Exit:
        return ret_val;
    }

    // Create camera list:
    ret_val = spinCameraListCreateEmpty(&hCameraList);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to create camera list; aborting"
            " with error %d\n",time(NULL),ret_val);

        // Exit:
        return ret_val;
    }

    // Retrieve list of cameras from the system:
    ret_val = spinSystemGetCameras(hSystem,hCameraList);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to retrieve camera list; aborting"
            " with error %d\n",time(NULL),ret_val);

        // Exit:
        return ret_val;
    }

    // Retrieve number of cameras:
    ret_val = spinCameraListGetSize(hCameraList,&numCameras);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to retrieve number of cameras;"
            " aborting with error %d\n",time(NULL),ret_val);

        // Exit:
        return ret_val;
    }

    // Check number of cameras:
    // (Only one camera is expected)
    if (numCameras != 1) {
        // Clear camera list before releasing system:
        ret_val = spinCameraListClear(hCameraList);

        // Check success:
        if (ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to clear camera list;"
                " aborting with error %d\n",time(NULL),ret_val);

            // Exit:
            return ret_val;
        }

        // Destroy camera list before releasing system:
        ret_val = spinCameraListDestroy(hCameraList);

        // Check success:
        if (ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to destroy camera list; aborting"
                " with error %d\n",time(NULL),ret_val);

            // Exit:
            return ret_val;
        }

        // Release system
        ret_val = spinSystemReleaseInstance(hSystem);\

        // Check success:
        if (ret_val != SPINNAKER_ERR_SUCCESS) {
            rt_printf("%d (INIT_CAM) Unable to release system instance;"
                " aborting with error %d\n",time(NULL),ret_val);

            // Exit:
            return ret_val;
        }

        // Print:
        rt_printf("%d (INIT_CAM) Found %d cameras (expected 1); aborting\n",\
            time(NULL),numCameras);

        // Exit:
        return -1;
    }

    // Select camera:
    ret_val = spinCameraListGet(hCameraList,0,&hCamera);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to retrieve camera from list;"
            " aborting with error %d\n",time(NULL),ret_val);

        // Exit:
        err_ret = ret_val;
    } else {
        // Initialize camera:
        ret_val = spinCameraInit(hCamera);

        // Check success:
        if (ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to initialize camera; aborting"
                " with error %d\n",time(NULL),ret_val);

            // Set return:
            err_ret = ret_val;
        }

        // Retrieve GenICam nodemap:
        ret_val = spinCameraGetNodeMap(hCamera,&hNodeMap);

        // Check success:
        if (ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to retrieve GenICam nodemap;"
                " aborting with error %d\n",time(NULL),ret_val);
            
            // Set return:
            err_ret = ret_val;
        }

        // Configure exposure:
        ret_val = config_cam_exp(hNodeMap);

        // Check success:
        if (ret_val != SPINNAKER_ERR_SUCCESS) {

            // Set return:
            err_ret = ret_val;
        }

        // De-initialize camera:
        ret_val = spinCameraDeInit(hCamera);

        // Check success:
        if (ret_val != SPINNAKER_ERR_SUCCESS) {
            // Print:
            rt_printf("%d (INIT_CAM) Unable to de-initialize camera;"
                " non-fatal error %d\n",time(NULL),ret_val);
            // Set return:
            err_ret = ret_val;
        }

        // Check success:
        if (ret_val != SPINNAKER_ERR_SUCCESS) {
            // Set return:
            err_ret = ret_val;
        }
    }

    // Release camera:
    ret_val = spinCameraRelease(hCamera);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to release camera; aborting with"
            " error %d\n",time(NULL),ret_val);

        // Exit:
        err_ret = ret_val;
    }

    // Clear camera list before releasing system:
    ret_val = spinCameraListClear(hCameraList);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to clear camera list;"
            " aborting with error %d\n",time(NULL),ret_val);

        // Exit:
        return ret_val;
    }

    // Destroy camera list before releasing system:
    ret_val = spinCameraListDestroy(hCameraList);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to destroy camera list; aborting"
            " with error %d\n",time(NULL),ret_val);

        // Exit:
        return ret_val;
    }

    // Release system:
    ret_val = spinSystemReleaseInstance(hSystem);

    // Check success:
    if (ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (INIT_CAM) Unable to release system instance; aborting"
            " with error %d\n",time(NULL),ret_val);

        // Exit:
        return ret_val;
    }

    // Exit:
    return err_ret;
}