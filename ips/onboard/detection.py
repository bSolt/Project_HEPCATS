# This script is responsible for loading and using the neural network model

import tensorflow as tf
import numpy as np
import os, rawpy, zlib
from PIL import Image
from FixedBufferReader import FixedBufferReader
from croppingScript import auto_crop
#from ips_helper import *

MODEL_FILE = "../models/winter_model_1.h5"
# INPUT_PIPE = "/dev/rtp0"
INPUT_PIPE = "/dev/pts/4"
THRESHOLD = 0.5
OUT_TEST = "output.npy"
BYTES = 9861950; #This is for the testing image
# BYTES = 2304000

model = tf.keras.models.load_model(MODEL_FILE)

#The program will loop while run is True
run = True
# Open the input pipe for reading and writing
# pipe = open(INPUT_PIPE,'r+b')


while(run):
	# raw = read_from_pipe(INPUT_PIPE)

	# PIPEScd proj	
	# Find out if this works and is blocking
	print(f"Reading from {INPUT_PIPE}")
	# First we open the pipe as a file-like object
	pbase = open(INPUT_PIPE,"rb")
	pid = FixedBufferReader(pbase.raw, read_size=BYTES)
	# Then we peek at the file to cause a block to wait for image data
	pid.peek();
	# interpret the pipe content as a rawpy object
	raw = rawpy.imread(pid)
	print("Now processing raw image!")
	rgb_full = raw.postprocess()
	# KIAN'S FUNCTION
	# concerned about run time, might need to fix later
	# in the future we might 
	rgb_crop = auto_crop(rgb_full)

	# for now
	rgb_crop = rgb_full

	# resizing is done using the PIL.Image module
	rgb_small = np.array(
		Image.fromarray(rgb_crop).resize((256,256))
		)
	rgb_small = np.expand_dims(rgb_small,0)
	pred = model.predict(rgb_small)
	# MATT COMPRESSION
	# input has to be read-only, tests use
	# might need to change the input to this
	# might have to save to file, then read as read only type
	compr_stream = zlib.compress(rgb_crop,zlib.Z_BEST_COMPRESSION)

	print(f'[INFO] {100*pred[0][0]:.2f}% chance of Aurora detected in image from {INPUT_PIPE}')

	## Write to pipe part
	
	if (pred >= THRESHOLD):
		message = f'Aurora Detected with probability {100*pred[0][0]:.2f}%\0'
		pid.write(message.encode())
	
	else:
		message = b'[P] No Aurora was detected.\0'
		pid.write(message)
	print(f'[P] Message written to pipe, size = {len(message)} bytes')

	run = False

