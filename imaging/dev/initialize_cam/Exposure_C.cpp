//=============================================================================
// Copyright © 2017 FLIR Integrated Imaging Solutions, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of FLIR
// Integrated Imaging Solutions, Inc. ("Confidential Information"). You
// shall not disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with FLIR Integrated Imaging Solutions, Inc. (FLIR).
//
// FLIR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. FLIR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

/**
 *  @example Exposure_C.cpp
 *
 *  @brief Exposure_C.cpp shows how to set a custom exposure time on a device.
 *  It relies on information provided in the Enumeration_C, Acquisition_C, and
 *  NodeMapInfo_C examples.
 *
 *  This example shows the processes of preparing the camera, setting a custom
 *  exposure time, and restoring the camera to a normal state (without power
 *  cycling). Ensuring custom values do not fall out of range is also touched
 *  on.
 *
 *  Following this, we suggest familiarizing yourself with the
 *  ImageFormatControl_C example if you haven't already. ImageFormatControl_C
 *  is another example on camera customization that is shorter and simpler than
 *  many of the others. Once comfortable with Exposure_C and
 *  ImageFormatControl_C, we suggest checking out any of the longer, more
 *  complicated examples related to camera configuration: ChunkData_C,
 *  LookupTable_C, Sequencer_C, or Trigger_C.
 */

#include "SpinnakerC.h"
#include "stdio.h"
#include "string.h"

// Compiler warning C4996 suppressed due to deprecated strcpy() and sprintf()
// functions on Windows platform.
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    #pragma warning(disable : 4996)
#endif

// This macro helps with C-strings.
#define MAX_BUFF_LEN 256

// This function helps to check if a node is available and readable
bool8_t IsAvailableAndReadable(spinNodeHandle hNode, char nodeName[])
{
    bool8_t pbAvailable = False;
    spinError err = SPINNAKER_ERR_SUCCESS;
    err = spinNodeIsAvailable(hNode, &pbAvailable);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve node availability (%s node), with error %d...\n\n", nodeName, err);
    }

    bool8_t pbReadable = False;
    err = spinNodeIsReadable(hNode, &pbReadable);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve node readability (%s node), with error %d...\n\n", nodeName, err);
    }
    return pbReadable && pbAvailable;
}


// This function helps to check if a node is available and writable
bool8_t IsAvailableAndWritable(spinNodeHandle hNode, char nodeName[])
{
    bool8_t pbAvailable = False;
    spinError err = SPINNAKER_ERR_SUCCESS;
    err = spinNodeIsAvailable(hNode, &pbAvailable);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve node availability (%s node), with error %d...\n\n", nodeName, err);
    }

    bool8_t pbWritable = False;
    err = spinNodeIsWritable(hNode, &pbWritable);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve node writability (%s node), with error %d...\n\n", nodeName, err);
    }
    return pbWritable && pbAvailable;
}

// This function handles the error prints when a node or entry is unavailable or
// not readable/writable on the connected camera
void PrintRetrieveNodeFailure(char node[], char name[])
{
    printf("Unable to get %s (%s %s retrieval failed).\n\n", node, name, node);
}

// This function configures a custom exposure time. Automatic exposure is turned
// off in order to allow for the customization, and then the custom setting is
// applied.
spinError ConfigureExposure(spinNodeMapHandle hNodeMap)
{
    spinError err = SPINNAKER_ERR_SUCCESS;

    printf("\n\n*** CONFIGURING EXPOSURE ***\n\n");

    //
    // Turn off automatic exposure mode
    //
    // *** NOTES ***
    // Automatic exposure prevents the manual configuration of exposure
    // time and needs to be turned off.
    //
    // *** LATER ***
    // Exposure time can be set automatically or manually as needed. This
    // example turns automatic exposure off to set it manually and back on
    // in order to return the camera to its default state.
    //
    spinNodeHandle hExposureAuto = NULL;
    spinNodeHandle hExposureAutoOff = NULL;
    int64_t exposureAutoOff = 0;

    // Retrieve enumeration node from nodemap
    err = spinNodeMapGetNode(hNodeMap, "ExposureAuto", &hExposureAuto);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to disable automatic exposure (node retrieval). Aborting with error %d...\n", err);
        return err;
    }

    // Retrieve entry node from enumeration node
    if (IsAvailableAndReadable(hExposureAuto, "ExposureAuto"))
    {
        err = spinEnumerationGetEntryByName(hExposureAuto, "Off", &hExposureAutoOff);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to disable automatic exposure (enum entry retrieval). Aborting with error %d...\n", err);
            return err;
        }
    }
    else
    {
        PrintRetrieveNodeFailure("node", "ExposureAuto");
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Retrieve integer value from entry node
    if (IsAvailableAndReadable(hExposureAutoOff, "ExposureAutoOff"))
    {
        err = spinEnumerationEntryGetIntValue(hExposureAutoOff, &exposureAutoOff);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to disable automatic exposure (enum entry int value retrieval). Aborting with error %d...\n", err);
            return err;
        }
    }
    else
    {
        PrintRetrieveNodeFailure("entry", "ExposureAuto 'Off'");
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Set integer as new value for enumeration node
    if (IsAvailableAndWritable(hExposureAuto, "ExposureAuto"))
    {
        err = spinEnumerationSetIntValue(hExposureAuto, exposureAutoOff);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to disable automatic exposure (enum entry setting). Aborting with error %d...\n", err);
            return err;
        }
        printf("Automatic exposure disabled...\n");
    }
    else
    {
        PrintRetrieveNodeFailure("node", "ExposureAuto");
        return SPINNAKER_ERR_ACCESS_DENIED;
    }
    //
    // Set exposure time manually; exposure time recorded in microseconds
    //
    // *** NOTES ***
    // It is ensured that the desired exposure time does not exceed the maximum.
    // Exposure time is counted in microseconds - this can be found out either
    // by retrieving the unit with the spinFloatGetUnit() methods or by
    // checking SpinView.
    //
    spinNodeHandle hExposureTime = NULL;
    double exposureTimeMax = 0.0;
    // SETTING EXPOSURE TIME HERE
    double exposureTimeToSet = 200000.0;

    // Retrieve exposure time node
    err = spinNodeMapGetNode(hNodeMap, "ExposureTime", &hExposureTime);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to set exposure time. Aborting with error %d...\n", err);
        return err;
    }

    // Retrieve maximum value
    if (IsAvailableAndReadable(hExposureTime, "ExposureTime"))
    {
        err = spinFloatGetMax(hExposureTime, &exposureTimeMax);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to set exposure time. Aborting with error %d...\n", err);
            return err;
        }
    }
    else
    {
        PrintRetrieveNodeFailure("node", "ExposureTime");
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Ensure desired exposure time does not exceed maximum
    if (exposureTimeToSet > exposureTimeMax)
    {
        exposureTimeToSet = exposureTimeMax;
    }

    // Set desired exposure time as new value
    if (IsAvailableAndWritable(hExposureTime, "ExposureTime"))
    {
        err = spinFloatSetValue(hExposureTime, exposureTimeToSet);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to set exposure time. Aborting with error %d...\n", err);
            return err;
        }

        printf("Exposure time set to %f us...\n", exposureTimeToSet);
    }
    else
    {
        PrintRetrieveNodeFailure("node", "ExposureTime");
        return SPINNAKER_ERR_ACCESS_DENIED;
    }
    return err;
}
// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo_C example for more in-depth comments on
// printing device information from the nodemap.
spinError PrintDeviceInfo(spinNodeMapHandle hNodeMap)
{
    spinError err = SPINNAKER_ERR_SUCCESS;
    unsigned int i = 0;

    printf("\n*** DEVICE INFORMATION ***\n\n");

    // Retrieve device information category node
    spinNodeHandle hDeviceInformation = NULL;

    err = spinNodeMapGetNode(hNodeMap, "DeviceInformation", &hDeviceInformation);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve node. Non-fatal error %d...\n\n", err);
        return err;
    }

    // Retrieve number of nodes within device information node
    size_t numFeatures = 0;

    if (IsAvailableAndReadable(hDeviceInformation, "DeviceInformation"))
    {
        err = spinCategoryGetNumFeatures(hDeviceInformation, &numFeatures);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to retrieve number of nodes. Non-fatal error %d...\n\n", err);
            return err;
        }
    }
    else
    {
        PrintRetrieveNodeFailure("node", "DeviceInformation");
        return SPINNAKER_ERR_ACCESS_DENIED;
    }

    // Iterate through nodes and print information
    for (i = 0; i < numFeatures; i++)
    {
        spinNodeHandle hFeatureNode = NULL;

        err = spinCategoryGetFeatureByIndex(hDeviceInformation, i, &hFeatureNode);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to retrieve node. Non-fatal error %d...\n\n", err);
            continue;
        }

        spinNodeType featureType = UnknownNode;

        //get feature node name
        char featureName[MAX_BUFF_LEN];
        size_t lenFeatureName = MAX_BUFF_LEN;
        err = spinNodeGetName(hFeatureNode, featureName, &lenFeatureName);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            strcpy(featureName, "Unknown name");
        }

        if (IsAvailableAndReadable(hFeatureNode, featureName))
        {
            err = spinNodeGetType(hFeatureNode, &featureType);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                printf("Unable to retrieve node type. Non-fatal error %d...\n\n", err);
                continue;
            }
        }
        else
        {
            printf("%s: Node not readable\n", featureName);
            continue;
        }

        char featureValue[MAX_BUFF_LEN];
        size_t lenFeatureValue = MAX_BUFF_LEN;

        err = spinNodeToString(hFeatureNode, featureValue, &lenFeatureValue);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            strcpy(featureValue, "Unknown value");
        }

        printf("%s: %s\n", featureName, featureValue);
    }
    printf("\n");

    return err;
}


// This function acts as the body of the example; please see NodeMapInfo_C
// example for more in-depth comments on setting up cameras.
spinError RunSingleCamera(spinCamera hCam)
{
    spinError err = SPINNAKER_ERR_SUCCESS;

    // Retrieve TL device nodemap and print device information
    spinNodeMapHandle hNodeMapTLDevice = NULL;

    err = spinCameraGetTLDeviceNodeMap(hCam, &hNodeMapTLDevice);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve TL device nodemap. Non-fatal error %d...\n\n", err);
    }
    else
    {
        err = PrintDeviceInfo(hNodeMapTLDevice);
    }

    // Initialize camera
    err = spinCameraInit(hCam);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to initialize camera. Aborting with error %d...\n\n", err);
        return err;
    }

    // Retrieve GenICam nodemap
    spinNodeMapHandle hNodeMap = NULL;

    err = spinCameraGetNodeMap(hCam, &hNodeMap);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve GenICam nodemap. Aborting with error %d...\n\n", err);
        return err;
    }

    // Configure exposure
    err = ConfigureExposure(hNodeMap);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        return err;
    }
    // Deinitialize camera
    err = spinCameraDeInit(hCam);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to deinitialize camera. Non-fatal error %d...\n\n", err);
    }

    return err;
}

// Example entry point; please see Enumeration_C example for more in-depth
// comments on preparing and cleaning up the system.
int main(/*int argc, char** argv*/)
{
    spinError errReturn = SPINNAKER_ERR_SUCCESS;
    spinError err = SPINNAKER_ERR_SUCCESS;
    unsigned int i = 0;

    
    // Print application build information
    printf("Application build date: %s %s \n\n", __DATE__, __TIME__);

    // Retrieve singleton reference to system object
    spinSystem hSystem = NULL;

    err = spinSystemGetInstance(&hSystem);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve system instance. Aborting with error %d...\n\n", err);
        return err;
    }

    // Retrieve list of cameras from the system
    spinCameraList hCameraList = NULL;

    err = spinCameraListCreateEmpty(&hCameraList);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to create camera list. Aborting with error %d...\n\n", err);
        return err;
    }

    err = spinSystemGetCameras(hSystem, hCameraList);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve camera list. Aborting with error %d...\n\n", err);
        return err;
    }

    // Retrieve number of cameras
    size_t numCameras = 0;

    err = spinCameraListGetSize(hCameraList, &numCameras);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to retrieve number of cameras. Aborting with error %d...\n\n", err);
        return err;
    }

    printf("Number of cameras detected: %u\n\n", (unsigned int)numCameras);

    // Finish if there are no cameras
    if (numCameras == 0)
    {
        // Clear and destroy camera list before releasing system
        err = spinCameraListClear(hCameraList);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to clear camera list. Aborting with error %d...\n\n", err);
            return err;
        }

        err = spinCameraListDestroy(hCameraList);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to destroy camera list. Aborting with error %d...\n\n", err);
            return err;
        }

        // Release system
        err = spinSystemReleaseInstance(hSystem);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to release system instance. Aborting with error %d...\n\n", err);
            return err;
        }

        printf("Not enough cameras!\n");
        printf("Done! Press Enter to exit...\n");
        getchar();

        return -1;
    }

    // Run example on each camera
    for (i = 0; i < numCameras; i++)
    {
        printf("\nRunning example for camera %d...\n", i);

        // Select camera
        spinCamera hCamera = NULL;

        err = spinCameraListGet(hCameraList, i, &hCamera);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            printf("Unable to retrieve camera from list. Aborting with error %d...\n\n", err);
            errReturn = err;
        }
        else
        {
            // Run example
            err = RunSingleCamera(hCamera);
            if (err != SPINNAKER_ERR_SUCCESS)
            {
                errReturn = err;
            }
        }

        err = spinCameraRelease(hCamera);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
            errReturn = err;
        }

        printf("Camera %d example complete...\n\n", i);
    }

    // Clear and destroy camera list before releasing system
    err = spinCameraListClear(hCameraList);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to clear camera list. Aborting with error %d...\n\n", err);
        return err;
    }

    err = spinCameraListDestroy(hCameraList);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to destroy camera list. Aborting with error %d...\n\n", err);
        return err;
    }

    // Release system
    err = spinSystemReleaseInstance(hSystem);
    if (err != SPINNAKER_ERR_SUCCESS)
    {
        printf("Unable to release system instance. Aborting with error %d...\n\n", err);
        return err;
    }

    printf("\nDone! Press Enter to exit...\n");
    getchar();

    return errReturn;
}
