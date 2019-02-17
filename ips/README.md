# HEPCATS_IPS
All software relevant to Senior Design Project HEPCATS Image Processing Subsystem

This subsystem is responsible for all image processing for the project, which can be split up into two parts based on the hardware on which it runs: On-Board, and Ground Station.

## On-Board Software
On-board the spacecraft computer, raw images are read in, cropped and classified as containing aurora or not. If an aurora is detected, the image is then compressed and passed along for downlink.

### Cropping
Cropping of images automatically detects where the disc of the Earth lies in the image using circular segmentation techniques. Based on the detected location of this, the image is cropped such that the Earth will always be centered and a constant ratio of image size. If necesarry, empty pixels are replaced with black.

### Auroral Detection
Auroral detection is achieved through the application of a Pre-Trained, Deep, Neural Network (PTDNN) and a custom classifier neural network. 

The pre-trained network is the keras implementation of the [Xception architecture](https://arxiv.org/abs/1610.02357), which has weights pre-trained on the ImageNET database. Before being processed in this network, the images are downsampled to a constant size of (256,256). This network can be called the feature detector network, because its output is vector of 2048 abstract features with cross section of 8 by 8. 

The custom classifer operates on these features and determines the likelihood of an Auroral substorm being present in the image. This network was trained on thousands of manually labeled images taken by the NASA [POLAR](https://pwg.gsfc.nasa.gov/polar/) and [IMAGE](https://image.gsfc.nasa.gov/) missions in UV, using the Earth Viewing Camera, and Far Ultraviolet Imager --- Wideband Imaging Camera, respectivly. The structure of the classifier first collapses the 2048 length feautres to 256 length, then flattens them along the extra dimensions, the final layer is a single node representing the likelihood of a substorm. During training of the network, a dropout layer (0.5) is used to mitigate the risk of overfitting.

### Compression

The zlib library is used to compress images in the format of ...


## Ground Station Software
Software running on a simulated ground station is then responsible for generating Latitude and Longitude coordiates for the pixels in the images and estimating the furtherest Latitude extent of an Auroral substorm.


### Geolocation

Geolocation works by applying geometry to orbital parameters from a simulated orbit epehemeris. Geolocation assumes that the cropping done on-board is correct.


### Substorm Extent

I have no idea how this works.