# This script is responsible for five things
# First,  a raw image will be loaded in from a named pipe which can be passed in 
# Second, the raw image is cropped such that the earth should be centered
# Third,  the neural network detection model is applied to detect the pressence of an auroral substorm
# Fourth, image compression is applied to decrease the resulting file size if necesarry
# Fifth,  the final, compressed image is written back to the pipe if necessary
# When an auroral substorm is not detected, only the first three things will happen

# Author: Braden Solt
# Team members: Alex Baughman, Jordan Lerner, Matt Skogen, Kian Tanner
# Date Created: Feb-21-2019

# python dependencies
# import tensorflow as tf
import numpy as np
import os, sys, rawpy, zlib, cv2
# This one is a custom buffer reading object
from FixedBufferReader import FixedBufferReader
# This one is the cropping function based on circle segmentation
from croppingScript import auto_crop

def read_raw(pipe, read_size):
	row = 1920; col = 1200;
	raw_arr = np.frombuffer(
		os.read(pipe, read_size)
		,np.uint8).reshape((col,row))
	# blk_arr = cv2.cvtColor(raw_arr, cv2.COLOR_BayerBG2GRAY)
	rgb_arr = cv2.cvtColor(raw_arr, cv2.COLOR_BayerBG2RGB)

	return rgb_arr

# Use this option for testing
# IMAGE_FORMAT = "test"
# Use this option for IEU integration
IMAGE_FORMAT = "ieu"

# This will be the file containing the full neural network model
# MODEL_FILE = "../models/winter_model_1.h5"

# Get pipe name from sys.argv if necessary
if len(sys.argv)>1:
	COMM_PIPE = sys.argv[1]
else:
	# Assume this name as a default
	COMM_PIPE = "/dev/rtp0"

# Threshold for Aurora Detection
THRESHOLD = 0.5

# Read the neural network model from a file
# model = tf.keras.models.load_model(MODEL_FILE)

# Expected RAW image size
if ( IMAGE_FORMAT=='test' ):
	BYTES = 9861950; #This is for the testing image
	# Create the custom reading object for appropriate reading within rawpy
elif ( IMAGE_FORMAT=='ieu'):
	BYTES = 2304000 #This is expected image size, from Chris

# Create buffer reader object for input
# p0 =  open(COMM_PIPE, 'rb')
# p_in = FixedBufferReader(p0.raw, read_size=BYTES)

# Create Buffer writier object for output
# p_out = open(COMM_PIPE, 'wb')

pipe = os.open(COMM_PIPE, os.O_RDWR)

#The program will loop while run is True
# It is not designed to stop in its current state
run = True

# Infinite Loop
while(run):
	print("[P] Reading from {}".format(COMM_PIPE))
	
	# Peek at the file to cause a block and wait for image data to be input
	# p_in.peek();
	# interpret the pipe content as a rawpy object

	if ( IMAGE_FORMAT=='test' ):
		raw = rawpy.imread(pipe)
		# Convert the raw image to a uint8 numpy array
		rgb_arr = raw.postprocess(gamma=(1,1))
	elif ( IMAGE_FORMAT=='ieu'):
		rgb_arr = read_raw(pipe, BYTES)

	# KIAN'S FUNCTION
	# Run time is lacking, needs optimization
	print("[P] Now cropping... Cross your fingers")
	rgb_crop, pcode, ecode = auto_crop(rgb_arr)
	print("[P] Crop done: pcode = {} ecode = {}, now classifying image".format(pcode, ecode))
	# Stop the program if there's a cropping error.
	if (pcode!=0 or ecode!=0):
		print("[P] Cropping Error")
		# Set pred to 0 so that it is equivalent to aurora not found
		pred=0
	else:
		# resizing is done using the opencv function
		# rgb_small = cv2.resize(rgb_crop, (256,256))
		# expand dims to prepare for input to neural net
		# rgb_small = np.expand_dims(rgb_small,0)
		# apply neural net model
		# pred = model.predict(rgb_small)
		pred = 1

		print('[P] {:.2f}% chance of Aurora detected in image'.format(100*pred[0][0]))
	
	# Check if the auroral threshold is met or not
	if (pred > THRESHOLD):
		# MATT COMPRESSION
		# NOTE: Could not accieve adequate compression at this time
		# Current strategy: Encode image to jpeg, then apply zlib
		result, buf = cv2.imencode('.png', rgb_crop)
		compr_stream = zlib.compress(buf,zlib.Z_BEST_COMPRESSION)
		print("[P] Image compressed to size {}".format(len(compr_stream)))
		# Write to pipe part
		print("[P] Attempting to write to {}".format(COMM_PIPE))
		# First we write the size of the compressed buffer as a 32 bit unsigned integer
		os.write(pipe,
			np.uint32(
				len(compr_stream)
				)
			)
		# Then we write the compressed buffer
		os.write(pipe,compr_stream)
		print('[P] rgb array written, size = {} bytes'.format(len(compr_stream)))
	else:
		# If no aurora is detected, we simply write EOF (0x00) to the pipe instead
		os.write(pipe,np.uint32(0))
		print('[P] EOF written to pipe')

	# run = False