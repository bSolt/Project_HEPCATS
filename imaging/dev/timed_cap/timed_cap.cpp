/*
timed_cap.cpp
Written by: Christopher Peercy
University of Colorado
Department of Aerospace Engineering-Undergraduate Senior Projects
Project HEPCATS

This is a test program  which uses spinnaker SDK provided by 
Point Grey Research, for use with the FLIR Blackfly Camera.

This takes a picture every 10 seconds for 2 minutes, saving
each picture to the local folder. 

This is written using the spinnaker C++ API's

*/

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream>


//Declare namespaces for spinnaker APIs
//TODO: Doublecheck all these are actually needed, since the camera we use is a USB 3.1 cam

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

//Acquisition example checks the GEV heartbeat, skip this and related function calls since using USB 3.1

//Image Aquisition function:
/*
Needs pointers to the camera, nodes, and node maps
*/
int ten_by_two(CameraPtr pCam, INodeMap &nodeMap, INodeMap &nodeMapTLDevice)
{
int result = 0;
cout << endl << endl << "Taking images every 10 seconds for two minutes" << endl <<endl;


/*
For now, we'll set the acquisition mode to continuous, we'll probably want to change this depending on how the
RTOS handles image aquisition
*/
CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode"); //Get the current acquisisition mode from the camera

//We're skipping error checking for now, ideally, we want to check that acquisition mode is set correctly

//Now get entry node, or maybe this sets the mode to continuous in firmware??
CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");

//Here I think we're getting a value from the firmware that will tell the camera to be in continuous mode
int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

//We use the int that i think we got from firmware, and use it to tell the camera to be in continuous here:
ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

//Skip disabling GEV heartbeat

//This actually tells the camera to get images:

pCam->BeginAcquisition();

cout << "Commanding Image Capture" << endl;

//Get information from the Camera:

gcstring deviceSerialNumber("");

//I'm convinced at this point that the .GetNode method pulls info off the camera's firmware, because
//we invoke it here again.

	CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
	deviceSerialNumber=ptrStringSerial->GetValue();
	cout <<"Attempted to get serial number, was received as: " << deviceSerialNumber << endl;


/*
Getting the images will be performed in a loop. In a release version of the software,
loop size will probably be configurable depending on commanding
*/

const unsigned int k_numImages=10; //This would be modified and passed from the user.

for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
{
	//Try statement for each acquisition attempt
	//Read an image into the camera buffer:
		ImagePtr pResultImage = pCam->GetNextImage();

	//Skipping completion check, if we get goofy image output we'll add it in
		//get the images sizes for error checking
		size_t width=pResultImage->GetWidth();
		size_t height=pResultImage->GetHeight();
		cout << "Got an image (hopefully)," << imageCnt << ", width " << width << ",height " << height <<endl;

	//Mono 8 conversion is not compatible with our mission needs, so we'll have to choose a different mode here:
	//I'm also not sure what the HQ_LINEAR call is...
		ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR); 
	//Make a filename for saving the image	
		ostringstream filename;
		filename << "Acquisition-"<< deviceSerialNumber.c_str()<<"-" << imageCnt << ".jpg";
	//Save the image
		convertedImage->Save(filename.str().c_str());
		cout << "Image saved as" << filename.str() <<endl;

	//Release the image to prevent memory leak in camera buffer
		pResultImage->Release();
		cout << endl;
	//end Capture TRY
}//END FOR Capture loop
//Tell the camera we're done with aquisition
pCam->EndAcquisition();


return result;
} //END FUNCTION ten_by_two


int RunSingleCamera(CameraPtr pCam)
{
	int result=0;
	try 
	{
	INodeMap & nodeMapTLDevice = pCam -> GetTLDeviceNodeMap();
	
	//Initiallize camera
	pCam->Init();
	
	INodeMap & nodeMap = pCam->GetNodeMap();
	
	//Acquire images
	result = result | ten_by_two(pCam, nodeMap, nodeMapTLDevice);

	//Deinit camera
	pCam->DeInit();
	}
	catch (Spinnaker::Exception &e)
	{
	cout << "ERROR: " << e.what() << endl;
	result = -1;
	}
	return result;
}







//Main function to run the aquisitoin loop.

int main(int /*argc*/, char** /*argv*/)
{
//Check to see write permissions exist in current directory
	FILE *tempFile = fopen("test.txt", "w+");
	if (tempFile == NULL) //Check to see if we grabbed a file descripter
	{
		cout << "There was a problem with permissions, can't write to folder" <<endl;
	getchar();
	return -1;
	}
	fclose(tempFile);
	remove("test.txt"); //Clean up the test file
	int result = 0;
	
	//Skipping version printing
	
	//Get a pointer to the system instance
	SystemPtr system = System::GetInstance();
	//Use that pointer to find cameras attached to that instance
	CameraList camList= system->GetCameras();
	unsigned int numCameras = camList.GetSize();
	
	cout << "Detected " << numCameras << "cameras" <<endl <<endl;
	
	//if no cameras exist, exit
	/*
	if (numCameras == 0) 
	{
	camList.Clear();
	system->ReleaseInstance();
	
	cout << "NO CAMERAS DETECTED. EXITING" << endl;
	getchar();
	
	return -1;
	} //END IF no cameras
	*/
	//Get a NULL pointer for the camera
	CameraPtr pCam = NULL;
	//Skipping multicamera aquisistion loop (only using one camera)
	
	//Assign pointer to camera
	pCam=camList.GetByIndex(0);
	cout << endl << "BEGINING AQUISITION LOOP. THIS WILL TAKE TWO MINUTES" << endl;
	
	result = result | RunSingleCamera(pCam);
	
	cout << "Aquisition complete... Hopefully?" << endl;
	
	//Release the camera
	system->ReleaseInstance();
	
	cout << endl << "Press enter to exit" << endl;
	getchar();
	return result;
}  //EOF MAIN

