/*
Written By: Christopher Peercy
University of Colorado - Boulder
Aerospace Engineering Senior Projects
Project HEPCATS

GetImages.cpp
This function commands the camera to get images at a particular rate. 
This file heavily references and closely follows the Spinnaker SDK examples provided by 
Point Grey Research.

INPUTS: Imaging rate in Hz //defunct
Returns: int, 0 success, 1 error

*/

#include "SpinnakerC.h" //Main SDK headers
#include "stdio.h"
#include "string.h"
#include "stdlib.h" //Needed for memory allocation
//#include "fcntl.h" //Needed for writing buffer to files
#include "unistd.h" //Needed to facilitate waiting between pictures.
#include "time.h" //Needed for timestamping files.

//Suppress compiler warnings about strings...
#if defined WIN32 || defined _WIN32 || defined WIN64 || _WIN64
#pragma warning(disable : 4996)
#endif

//Extend the buffer length
#define MAX_BUFF_LEN 256







//Support function for printing device info
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







//Support function for printing device info
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








// This function handles the error prints when a node or entry is unavailable or
// not readable/writable on the connected camera
void PrintRetrieveNodeFailure(char node[], char name[])
{
    printf("Unable to get %s (%s %s retrieval failed).\n\n", node, name, node);
}









//Print device information
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









//Main Aquisistion function
spinError Acquire(spinCamera hCam, spinNodeMapHandle hNodeMap, spinNodeMapHandle hNodeMapTLDevice)
{

	spinError err = SPINNAKER_ERR_SUCCESS;
	
	//Take a picture
	err = spinCameraBeginAcquisition(hCam);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Couldn't start acquisition. Sorry bout that...");
		return err;
	}
	else
	{
		printf("Successfully commanded camera acquisition start");
	}
	
	spinImage hResultImage=NULL;
	err = spinCameraGetNextImage(hCam, &hResultImage);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Tried to get the picture but couldn't\n");
		return err;
	}
	//If we have a picture, let the user know.
	printf("Retreived an image\n");
	
	//Convert the buffer to the right pixelspace
	spinImage hConvertedImage = NULL;
	
	err = spinImageCreateEmpty(&hConvertedImage);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("FATAL: Couldn't create an empty image to hold converted image \n");
		return err;
	}
	
	err = spinImageConvert(hResultImage, PixelFormat_RGB8, hConvertedImage);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("FATAL: Image conversion failed\n");
		//return err;
	}

	//For now, save to a file
	char filename[MAX_BUFF_LEN];
	time_t CurrentTime=time(NULL);
	char* strTime;
	strTime=ctime(&CurrentTime);
	
	sprintf(filename,"Image" );
	err = spinImageSave(hConvertedImage, filename, JPEG);
	sleep(1);
		if(err != SPINNAKER_ERR_SUCCESS)
		{
		printf("Failed to save the Image\n");
		return err;
		}
		else
		{
		printf("Saved the image to current directory\n");
		}
	//Clean up, clean up, everybody do your share
	printf("Cleaning up image and camera handles\n");
	err = spinImageDestroy(hConvertedImage);
	err = spinImageDestroy(&hResultImage);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
                printf("NON-FATAL: Could not destroy result image\n");
	}
	err = spinCameraEndAcquisition(hCam);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Could not end image aquisition! Future images may not capture\n");
	}
	else
	printf("Camera handles cleaned up\n");

	return err;
} //END OF FUNCTION ACQUIRE










int main(int argc, char** argv)
{
	spinError err=SPINNAKER_ERR_SUCCESS;
	//check permissions
	{
	FILE *temp;
	temp=fopen("test.txt","w+");
	if (temp == NULL)
	{
		printf("Cannot save images in current folder. Invalid permissions\n");
		return SPINNAKER_ERR_ACCESS_DENIED;
	}
	else
	{
		printf("Permissions check complete. Write permissions OK");
	}
	fclose(temp);
	remove("test.txt");
	}
	//Make a system object
	spinSystem hSystem=NULL;
	
	err=spinSystemGetInstance(&hSystem);
	
	//Get a camera list from the system
	spinCameraList hCameraList=NULL;
	err = spinCameraListCreateEmpty(&hCameraList);
	err=spinSystemGetCameras(hSystem,hCameraList);
	
	size_t numCameras=0;
	err = spinCameraListGetSize(hCameraList, &numCameras);
	if (numCameras == 0)
	{
		err = spinCameraListClear(hCameraList);
		err = spinCameraListDestroy(hCameraList);
		err = spinSystemReleaseInstance(hSystem);
		printf("No cameras attached.Hook up a camera dummy \n");
		return -1;
	}
	//We'll initialize the camera here
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//FROM RUN SINGLE CAMERA//
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	spinCamera hCamera = NULL;
	err = spinCameraListGet(hCameraList, 0, &hCamera); //Only one camera, lives at index 0
	//Need TL nodemaps
	spinNodeMapHandle hNodeMapTLDevice=NULL;
        err = spinCameraGetTLDeviceNodeMap(hCamera, &hNodeMapTLDevice);
	
	//Print device information to get initial state
	printf("Getting initial camera state:\n");
	err = PrintDeviceInfo(hNodeMapTLDevice);

        //Initialize camera
        err= spinCameraInit(hCamera);
        if(err!= SPINNAKER_ERR_SUCCESS)
        {
		//If we can't initiallize, yell at the user a bit
                printf("Could not initialize camera. Aborting cause Chris' code is probs broken \n");
                printf("Or someone just doesn't know how to use a USB port\n");
                printf("Make sure the camera is NOT plugged into the paddleboard\n");
                printf("The camera has to be plugged into the 3.1 port on the IEU\n");
                printf("The first is more likely tho, cause I have like, ZERO idea what I'm doing\n");
                return err;
        }

        //Get the full nodeMap using GenICam
        spinNodeMapHandle hNodeMap = NULL;

        err = spinCameraGetNodeMap(hCamera, &hNodeMap);
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//END FROM RUN SINGLE CAMERA//

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//FROM MAIN ACQUIRE FUNCTION//

        //Set the camera to single aquisition mode

        //Make node handles
        spinNodeHandle hAcquisitionMode = NULL;
        spinNodeHandle hAcquisitionModeSingle = NULL;
        int64_t acquisitionModeSingle = 0;
	
        err = spinNodeMapGetNode(hNodeMap, "AcquisitionMode", &hAcquisitionMode);
        printf("Got acquisition mode node map\n");
        err = spinEnumerationGetEntryByName(hAcquisitionMode, "SingleFrame", &hAcquisitionModeSingle);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
                printf("Error setting camera to single frame aquisition\n");
                return err;
        }
        else
        {
                printf("Single frame aquisition set successfully\n");
        }

	
        err = spinEnumerationEntryGetIntValue(hAcquisitionModeSingle, &acquisitionModeSingle);
        //Set the integer as the new enumeration value:
        err = spinEnumerationSetIntValue(hAcquisitionMode, acquisitionModeSingle);
        if (err != SPINNAKER_ERR_SUCCESS)
        {
                printf("Could not set camera to single frame");
                return err;
        }
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
	//END FROM MAIN AQUIRE FUNCTION//

	//Done with init stuff, get camera info again
	printf("Finished setting camera to SFA state\n");

	
	//Ready to aquire. 
	int imageCount = 10; //Number of pictures we want to take
        int imageInterval = 1; //Number of seconds to wait between images.
	for(int i=0; i < imageCount; i++)
	{
		printf("Taking picture %d \n", i);
		err = Acquire(hCamera, hNodeMap, hNodeMapTLDevice);
		if (err != SPINNAKER_ERR_SUCCESS)
		{
			printf("Fatal image aquisition error, dumping device state:\n");
			err = PrintDeviceInfo(hNodeMapTLDevice);
			return -1;
		}
		printf("Waiting for %d seconds\n", imageInterval);
		//wait for a the interval
		sleep(imageInterval);
	}
	//Clean up the system and release camera referances
	
	err = spinCameraDeInit(hCamera);
	err=spinCameraRelease(hCamera);
	err=spinCameraListClear(hCameraList);
	err = spinCameraListDestroy(hCameraList);
	
	//Release the system
	err = spinSystemReleaseInstance(hSystem);
	printf("\n Extraordinary. Everything is done.\n");
	return err;
}

