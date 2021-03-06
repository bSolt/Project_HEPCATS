/*
Written By: Christopher Peercy
University of Colorado - Boulder
Aerospace Engineering Senior Projects
Project HEPCATS

GetImages.cpp
This function commands the camera to get images at a particular rate. 
This file heavily references and closely the Spinnaker SDK examples provided by 
Point Grey Research.

INPUTS: Imaging rate in Hz
Returns: int, 0 success, 1 error

*/

#include "SpinnakerC.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h" //Needed for memory allocation
#include "fcntl.h" //Needed for writing buffer to files
#include "unistd.h" //Needed to facilitate waiting between pictures.
#include "time.h" //Needed for timestamping files.

//Suppress compiler warnings about strings...
#if defined WIN32 || defined _WIN32 || defined WIN64 || _WIN64
#pragma warning(disable : 4996)
#endif

//Extend the buffer length
#define MAX_BUFF_LEN 256

//Main Aquisistion function
spinError Acquire(spinCamera hCam, spinNodeMapHandle hNodeMap, spinNodeMapHandle hNodeMapTLDevice)
{

	spinError err = SPINNAKER_ERR_SUCCESS;
	printf("\n STARTING IMAGE AQUISISTION \n");

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
	
	//Take a picture
	err = spinCameraBeginAcquisition(hCam);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Couldn't start acquisition. Sorry bout that...");
		return err;
	}
	
	spinImage hResultImage=NULL;
	err = spinCameraGetNextImage(hCam, &hResultImage);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Tried to get the picture but couldn't\n");
		return err;
	}
	//If we have a picture, let the user know.
	printf("Retreived an image");
	
	//Convert the buffer to the right pixelspace
	spinImage hConvertedImage = NULL;
	
	err = spinImageCreateEmpty(&hConvertedImage);
	err = spinImageConvert(hResultImage, PixelFormat_BayerRG8, hConvertedImage);



	//For now, save to a file
	char filename[MAX_BUFF_LEN];
	sprintf(filename,"newImage");
	err = spinImageSave(hConvertedImage, filename, RAW);
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
	err = spinCameraEndAcquisition(hCam);
	printf("Camera handles cleaned up");

	


	//Write raw to buffer
        
		//Open the file read only
		FILE *imgFile=fopen(filename, O_RDONLY);
		if (imgFile < 0)
			printf("Could not get file descriptor for raw image");
		else
		{
		printf("Got file descriptor for image file");
		size_t imageSize=2304000;
		//Open the buffer file for writing
		const char *Buff_location="/dev/rtp1/";
		int destFile = open(Buff_location, O_CREAT, O_WRONLY);
		int ferr;
		ferr = write(destFile,imgFile, imageSize); //All raw images are exactly 2304000 bytes so long as bRG8 is set as pixelspace
		if (ferr > 0)
			printf("\nSuccessfuly wrote %d to buffer\n", ferr);
		else
			printf("\n Could not write to buffer\n");
		ferr=fclose(imgFile);
		ferr=close(destFile);
		}



//Cleanup the result image
	err = spinImageDestroy(&hResultImage);

	return err;
} //END OF FUNCTION ACQUIRE

spinError RunSingleCamera(spinCamera hCam)
{
	spinError err= SPINNAKER_ERR_SUCCESS;
	
	//Need the transport layer nodemap for later:
	spinNodeMapHandle hNodeMapTLDevice=NULL;
	err = spinCameraGetTLDeviceNodeMap(hCam, &hNodeMapTLDevice);
	
	//Initialize camera
	err= spinCameraInit(hCam);
	if(err!= SPINNAKER_ERR_SUCCESS)
	{
		printf("Could not initialize camera. Aborting cause Chris' code is probs broken \n");
		printf("Or someone just doesn't know how to use a USB port\n");
		printf("The first is more likely tho, cause I have like, ZERO idea what I'm doing\n");
		return err;
	}
	
	//Get the full nodeMap using GenICam
	spinNodeMapHandle hNodeMap = NULL;
	
	err = spinCameraGetNodeMap(hCam, &hNodeMap);
	
	//Call that good ol acquisistion function now and get a picture.
	err = Acquire(hCam, hNodeMap, hNodeMapTLDevice);
	
	//Now that we have a picture, we gotta release the camera
	err = spinCameraDeInit(hCam);
	
	return err;
} //END OF FUNCTION RunSingleCamera

int main(/*int argc, char** argv*/)
{
	spinError err=SPINNAKER_ERR_SUCCESS;
	

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
	// Get some images every so often.
	int imageCount = 10; //Number of pictures we want to take
	int imageInterval = 5; //Number of seconds to wait between images.
	for(int i=0; i < imageCount; i++)
	{
		printf("Taking picture %d \n", i);
		spinCamera hCamera = NULL;
		err = spinCameraListGet(hCameraList, 0, &hCamera); //pass 0 as second argument since we only have 1 camera
		err = RunSingleCamera(hCamera);
		
		//Release the camera
		err = spinCameraRelease(hCamera);
		
		//wait for a the interval
		sleep(imageInterval);
	}
	//Clean up the system
	err=spinCameraListClear(hCameraList);
	err = spinCameraListDestroy(hCameraList);
	
	//Release the system
	err = spinSystemReleaseInstance(hSystem);
	printf("/n Extraordinary. Everything is done.");
	return err;
}

