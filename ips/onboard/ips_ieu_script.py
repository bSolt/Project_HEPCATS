# This script is responsible for five things
# First,  a raw image will be loaded in from a named pipe which can be passed in 
# Second, the raw image is cropped such that the earth should be centered
# Third,  the neural network detection model is applied to detect the pressence of an auroral substorm
# Fourth, image compression is applied to decrease the resulting file size if necesarry
# Fifth,  the final, compressed image is written back to the pipe if necessary
# When an auroral substorm is not detected, only the first three things will happen

# Author: Braden Solt
# Team members: Alex Baughman, Jordan Lerner, Matt Skogen, Kian Tanner
# Date Created: Feb-18-2019

# python dependencies
import tensorflow as tf
import numpy as np
import os, sys, rawpy, zlib, cv2
# This one is a custom buffer reading object
from FixedBufferReader import FixedBufferReader
# This one is the cropping function based on circle segmentation
from croppingScript import auto_crop

# This will be the file containing the full neural network model
MODEL_FILE = "../models/winter_model_1.h5"

# Get pipe name from sys.argv if necessary
if len(sys.argv)>1:
	COMM_PIPE = sys.argv[1]
else:
	# Assume this name as a default
	COMM_PIPE = "/dev/rtp/0"

# Threshold for Aurora Detection
THRESHOLD = 0.5

# Expected RAW image size
BYTES = 9861950; #This is for the testing image
# BYTES = 10253806;
# BYTES = 2304000 #This is expected image size, from Chris

model = tf.keras.models.load_model(MODEL_FILE)

#The program will loop while run is True
# It is not designed to stop in its current state
run = True
# Create buffer reader object for input
p0 =  open(COMM_PIPE, 'rb')
# Create the custom reading object for appropriate reading within rawpy
p_in = FixedBufferReader(p0.raw, read_size=BYTES)
# Create Buffer writier object for output
p_out = open(COMM_PIPE, 'wb')

# Infinite Loop
while(run):
	print(f"[P] Reading from {COMM_PIPE}")
	
	# Peek at the file to cause a block and wait for image data to be input
	p_in.peek();
	# interpret the pipe content as a rawpy object
	print("[P] Now processing raw image!")
	raw = rawpy.imread(p_in)
	# Convert the raw image to a uint8 numpy array
	rgb_full = raw.postprocess()

	# KIAN'S FUNCTION
	# Run time is lacking, needs optimization
	print("[P] Now cropping... Cross your fingers")
	rgb_crop, pcode, ecode = auto_crop(rgb_full)
	print(f"[P] Crop done: pcode = {pcode} ecode = {ecode}, now classifying image")

	# resizing is done using the opencv function
	rgb_small = cv2.resize(rgb_crop, (256,256))
	# expand dims to prepare for input to neural net
	rgb_small = np.expand_dims(rgb_small,0)
	# apply neural net model
	pred = model.predict(rgb_small)

	print(f'[P] {100*pred[0][0]:.2f}% chance of Aurora detected in image from {COMM_PIPE}')
	
	if (pred > THRESHOLD):
		# MATT COMPRESSION
		# NOTE: Could not accieve adequate compression at this time
		# Current strategy: Encode image to jpeg, then apply zlib
		result, buf = cv2.imencode('.jpg', rgb_crop)
		compr_stream = zlib.compress(buf,zlib.Z_BEST_COMPRESSION)

		# Write to pipe part
		print(f"[P] Attempting to write to {COMM_PIPE}")
		# First we write the size of the compressed buffer as a 32 bit unsigned integer
		p_out.write(
			np.uint32(
				len(compr_stream)
				)
			)
		# Then we write the compressed buffer
		p_out.write(compr_stream)
		print(f'[P] rgb array written to {COMM_PIPE}, size = {len(compr_stream)} bytes')
	else:
		# If no aurora is detected, we simply write EOF (0x00) to the pipe instead
		p_out.write(np.uint32(0))
		print(f'[P] EOF written to pipe')
