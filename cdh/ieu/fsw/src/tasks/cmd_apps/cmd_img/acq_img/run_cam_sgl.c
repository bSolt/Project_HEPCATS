///////////////////////////////////////////////////////////////////////////////
//
// Run Camera Single
//
// Function responsible for acquiring an image from the camera with singe
// mode acquisition.
//
// -------------------------------------------------------------------------- /
//
// Dependencies:
// - Xenomai / Cobalt
// - Spinnaker
//
// Input Arguments:
// - hCamera
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
#include <alchemy/sem.h>  // Semaphore services

// Spinnaker libraries:
#include <SpinnakerC.h> // FLIR machine vision camera interface definitions

// Semaphore definitions:
RT_SEM new_img_sem; // For run_cam_sgl and read_usb_img task
                    // synchronization 

spinError run_cam_sgl(spinCamera hCamera, spinNodeMapHandle hNodeMap,\
    spinNodeMapHandle hNodeMapTLDevice) {

    // Definitions and initializations:
    spinError spin_ret_val = SPINNAKER_ERR_SUCCESS;

    char filename[50];

    spinImage hResultImage = NULL;

    spinImage hConvertedImage = NULL;

    // Take a picture
    spin_ret_val = spinCameraBeginAcquisition(hCamera);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Unable to start image acquisition"
            " ; aborting with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    } else {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Starting image acquisition\n",time(NULL));
    }
    
    // Get acquired image:
    spin_ret_val = spinCameraGetNextImage(hCamera,&hResultImage);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Image acquisition failed; aborting with"
            " error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    } else {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Image acquisition successful\n",\
            time(NULL));
    }
    
    // Convert the buffer to the right pixelspace:    
    spin_ret_val = spinImageCreateEmpty(&hConvertedImage);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Unable to create an empty image to hold"
            " converted image; aborting with error %d\n",time(NULL),\
            spin_ret_val);

        // Exit:
        return spin_ret_val;
    }

    // Convert image:
    spin_ret_val = spinImageConvert(hResultImage,PixelFormat_RGB8,\
        hConvertedImage);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Unable to convert image with error %d\n",\
            time(NULL),spin_ret_val);
    }

    // Create file name:
    sprintf(filename,"/tmp/img.raw");

    // Save image to file:
    spin_ret_val = spinImageSave(hConvertedImage,filename,RAW);

    // Check success:
    if(spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Unable to save image to file; aborting"
            " with error %d\n",time(NULL),spin_ret_val);

        // Exit:
        return spin_ret_val;
    } else {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Image saved to file; notifying"
            " read_usb_img task\n",time(NULL));

        // Signal read_usb_img task that new image is ready to read:
        rt_sem_v(&new_img_sem);
    }

    // Destroy image:
    spin_ret_val = spinImageDestroy(hConvertedImage);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        //rt_printf("%d (RUN_CAM_SGL) Unable to destroy image with error %d\n",\
            time(NULL),spin_ret_val);
    }

    // Destroy image:
    spin_ret_val = spinImageDestroy(&hResultImage);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        //rt_printf("%d (RUN_CAM_SGL) Unable to destroy image with error %d\n",\
            time(NULL),spin_ret_val);
    }

    // End acquisition:
    spin_ret_val = spinCameraEndAcquisition(hCamera);

    // Check success:
    if (spin_ret_val != SPINNAKER_ERR_SUCCESS) {
        // Print:
        rt_printf("%d (RUN_CAM_SGL) Unable to end image acquisition"
            " with error %d\n",time(NULL),spin_ret_val);
    }
    
    // Return:
    return spin_ret_val;
}