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
	printf("Got acquisition mode");
	err = spinEnumerationGetEntryByName(hAcquisitionMode, "SingleFrame", &hAcquisitionModeSingle);
	if (err != SPINNAKER_ERR_SUCCESS)
	{
		printf("Error setting camera to single frame aquisition");
		return err;
	}
	else
	{
		printf("Single frame aquisition set successfully");
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
		printf("Tried to get the picture but couldn't read the buffer");
		return err;
	}
	//If we have a picture, let the user know.
	size_t width = 0;
	size_t height = 0;
	printf("Retreived an image with width %u, ",(unsigned int)width);
	printf("height %u. \n", (unsigned int)height);
	
	//Try to get the rawImage buffer
	int fd = open("testOutputRaw", O_WRONLY);
		if(fd < 0)
			printf("Couldn't get file descriptor to save unconverted buffer. Skipping");
		else
		{
			if(write(fd,hResultImage, 4000000) < 0)
			{
				printf("Tried to write unconverted image buffer to file but failed. Continuing.\n");
				int fderr=close(fd);
				if(fderr)
					printf("there was a goof up closing the buffer file");
			}
			else
			{
			printf("Successfully wrote image buffer to file testOutput. Hopefully this will be useful");
			int fderr=close(fd);
			if(fderr)
				printf("There was a goof up closing the buffer file, we may have lost the data. Not my fault.\n");
			}
		}
	//Convert the buffer to the right pixelspace
	spinImage hConvertedImage = NULL;
	
	err = spinImageCreateEmpty(&hConvertedImage);
	err = spinImageConvert(hResultImage, PixelFormat_BayerRG8, hConvertedImage);
	
	//Try to write the converted buffer to a file
	        fd = open("testOutputConverted", O_WRONLY);
                if(fd < 0)
                        printf("Couldn't get file descriptor to save converted buffer. Skipping\n");
                else
                {
                        if(write(fd,hConvertedImage, 4000000) < 0)
                        {
                                printf("Tried to write converted image buffer to file but failed. Continuing.\n");
                                int fderr=close(fd);
                                if(fderr)
                                        printf("there was a goof up closing the buffer file\n");
                        }
                        else
                        {
                        printf("Successfully wrote image buffer to file testOutputConverted. \nHopefully this will be useful");
                        int fderr=close(fd);
                        if(fderr)
			{                        
        			printf("There was a goof up closing the buffer file, we may have lost the data. \nNot my fault.");
			}                        
}
                }
	//For now, save to a file
	time_t now;
	time(&now);
	char filename[MAX_BUFF_LEN];
	sprintf(filename,"Acquisition-C-GetImage-Ex_%s",ctime(&now));
	err = spinImageSave(hConvertedImage, filename, JPEG);//Need to see what hConvertedImage actually is.
	printf("Saved the image. Let's not set our expectations too high tho, something's probably broken.\n");
	
	//Clean up, clean up, everybody do your share
	err = spinImageDestroy(hConvertedImage);
	err = spinImageRelease(hResultImage);
	err = spinCameraEndAcquisition(hCam);
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
	spinError errReturn=SPINNAKER_ERR_SUCCESS; 
	

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

