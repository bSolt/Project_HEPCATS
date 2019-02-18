# Setup
import cv2
import numpy as np


def auto_crop(im_array):
    # Meta
    fileName = 'image6.png'

    # Predicted Radii
    startingRadius = 800
    radiusDelta = 25
    endingRadius = 1500

    # Detection Parameters
    blurSigma = 15
    sens = 15
    minAccumulatorVotes = 170
    startingAccumulatorVotes = 240
    accumulatorThresh = startingAccumulatorVotes

    # Output Image Parameters
    deltaCropRatio = 0.2
    BLACK = [0, 0, 0]
    borderPad = 300

    # fix image
    if (im_array.shape[2] > 1):
        I_bw = cv2.cvtColor(im_array,cv2.COLOR_RGB2GRAY)
    else:
        I_bw = im_array
    I_bw = cv2.GaussianBlur(I_bw, (blurSigma, blurSigma), 0)
    I_bw = cv2.GaussianBlur(I_bw, (blurSigma, blurSigma), 0)
    I_bw = cv2.GaussianBlur(I_bw, (blurSigma, blurSigma), 0)

    I_col = cv2.imread(fileName)
    I_col = cv2.copyMakeBorder(I_col, borderPad, borderPad, borderPad, borderPad, cv2.BORDER_CONSTANT, value=BLACK)


    # Find Circles
    while True:
        if accumulatorThresh < minAccumulatorVotes:
            accumulatorThresh = startingAccumulatorVotes
            startingRadius = startingRadius+radiusDelta
            
        if startingRadius > endingRadius:
            print("No circle under specified radius found")
            raise SystemExit(0)
        
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


    circles = np.uint16(np.around(circles))
    meanC = np.mean(circles, 1)
    meanX = int(np.round(meanC[0,0], 0))
    meanY = int(np.round(meanC[0,1], 0))
    meanR = int(np.round(meanC[0,2], 0))
    deltaCrop = int((deltaCropRatio+1)*meanR)
    height, width, depth = I_col.shape
    I_crop = I_col[meanY-deltaCrop:meanY+deltaCrop, meanX-deltaCrop:meanX+deltaCrop]
    cv2.imwrite("cropped.png", I_crop)

        
    # Pointing Check
    if (meanX-meanR) < borderPad:
        print("Point better. Earth is too far left.")
    elif (meanX+meanR) > (width-borderPad):
        print("Point better. Earth is too far right.")
    elif (meanY-meanR) < borderPad:
        print("Point better. Earth is too far up.")
    elif (meanY+meanR) > (height-borderPad):
        print("Point better. Earth is too far down.")
    else:
        print("Good pointing.")

    return I_crop
