# Setup
import cv2
import numpy as np


def auto_crop(im_array,
    startingRadius = 50, radiusDelta = 25, endingRadius = 300, minAccumulatorVotes = 150):
    # Radii Parameters
    radiusDelta = (endingRadius-startingRadius)//20

    # Detection Parameters
    blurSigma = 15
    sens = 15
    startingAccumulatorVotes = 240
    accumulatorThresh = startingAccumulatorVotes

    # Output Image Parameters
    deltaCropRatio = 0.2
    BLACK = [0, 0, 0]
    borderPad = 300

    # fix image
    I_bw = cv2.cvtColor(im_array, cv2.COLOR_RGB2GRAY)
    I_bw = cv2.GaussianBlur(I_bw, (blurSigma, blurSigma), 0)
    I_bw = cv2.GaussianBlur(I_bw, (blurSigma, blurSigma), 0)
    I_bw = cv2.GaussianBlur(I_bw, (blurSigma, blurSigma), 0)

    I_col = im_array
    I_col = cv2.copyMakeBorder(I_col, borderPad, borderPad, borderPad, borderPad, cv2.BORDER_CONSTANT, value=BLACK)


    # Find Circles
    errorCode = 0
    pointingCode = 0
    while True:
        if accumulatorThresh < minAccumulatorVotes:
            accumulatorThresh = startingAccumulatorVotes
            startingRadius = startingRadius+radiusDelta
            
        if startingRadius > endingRadius:
            print("No circle under specified radius found.")
            I_crop = 0
            errorCode = 1
            break
        
        circles = cv2.HoughCircles(I_bw, cv2.HOUGH_GRADIENT, 2, 5, param1=sens, param2=accumulatorThresh, minRadius=startingRadius-radiusDelta, maxRadius=startingRadius+radiusDelta)
        try:
            numCircles = len(circles[0,:])
        except TypeError:
            accumulatorThresh = accumulatorThresh - 5
            continue
        
        if len(circles[0,:]) > 10:
            accumulatorThresh = accumulatorThresh + 5
            continue
        elif len(circles[0,:]) < 3:
            accumulatorThresh = accumulatorThresh - 5
            continue
        else:
            break

    if errorCode == 0:
        circles = np.uint16(np.around(circles))
        meanC = np.mean(circles, 1)
        meanX = int(np.round(meanC[0,0], 0))
        meanY = int(np.round(meanC[0,1], 0))
        meanR = int(np.round(meanC[0,2], 0))
        deltaCrop = int((deltaCropRatio+1)*meanR)
        height, width, depth = I_col.shape
        I_crop = I_col[meanY-deltaCrop+borderPad:meanY+deltaCrop+borderPad, meanX-deltaCrop+borderPad:meanX+deltaCrop+borderPad]

        # Pointing Check
        if (meanX-meanR) < borderPad: # Need to point better, Earth is too far left
            pointingCode = 1
        elif (meanX+meanR) > (width-2*borderPad): # Need to point better, Earth is too far right
            pointingCode = 2
        elif (meanY-meanR) < borderPad: # Need to point better, Earth is too far up
            pointingCode = 3
        elif (meanY+meanR) > (height-2*borderPad): # Need to point better, Earth is too far down
            pointingCode = 4

    return (I_crop, pointingCode, errorCode)
    
    
# Script
# We only run this if this is the main file.
if __name__ == '__main__':

    fileName = 'image9.png'
    in_image = cv2.imread(fileName)
    [out_image, pointingError, errorCode] = auto_crop(in_image)

    if errorCode == 1:
        print("Error: No circles found.")
    else:
        print("Image cropped successflly.")
        cv2.imwrite("cropped.png", out_image)
        if pointingError != 0:
            print("Camera not pointing correctly, error code %s." % pointingError)

