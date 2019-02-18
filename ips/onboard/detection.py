# This script is responsible for loading and using the neural network model

import tensorflow as tf
import numpy as np
import os, sys, rawpy, zlib, cv2
from FixedBufferReader import FixedBufferReader
from croppingScript import auto_crop
#from ips_helper import *

MODEL_FILE = "../models/winter_model_1.h5"
# INPUT_PIPE = "/dev/rtp0"
if len(sys.argv)>2:
	INPUT_PIPE = sys.argv[1]
	OUTPUT_PIPE = sys.argv[2]
else:
	INPUT_PIPE = "/dev/pts/4"
	OUTPUT_PIPE = "/dev/pts/3"
THRESHOLD = 0.5

BYTES = 9861950; #This is for the testing image
# BYTES = 10253806;
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
	print(f"[P] Reading from {INPUT_PIPE}")
	# First we open the pipe as a file-like object
	pbase = open(INPUT_PIPE,"rb")
	pid = FixedBufferReader(pbase.raw, read_size=BYTES)
	# Then we peek at the file to cause a block to wait for image data
	pid.peek();
	# interpret the pipe content as a rawpy object
	raw = rawpy.imread(pid)

	pid.close()

	print("[P] Now processing raw image!")
	rgb_full = raw.postprocess()
	# KIAN'S FUNCTION
	# concerned about run time, might need to fix later
	# in the future we might 
	print("[P] Now cropping... Cross your fingers")
	rgb_crop, pcode, ecode = auto_crop(rgb_full)

	# for now
	# rgb_crop = rgb_full
	print(f"[P] Crop done pcode = {pcode} ecode = {ecode}, now classifying image")
	# resizing is done using the opencv function
	rgb_small = cv2.resize(rgb_full, (256,256))
	rgb_small = np.expand_dims(rgb_small,0)
	pred = model.predict(rgb_small)

	print(f'[P] {100*pred[0][0]:.2f}% chance of Aurora detected in image from {INPUT_PIPE}')

	# MATT COMPRESSION
	# input has to be read-only, tests use
	# might need to change the input to this
	# might have to save to file, then read as read only type

	# NOTE: Could not accieve adequate compression at this time

	pid = open(OUT_PIPE,'wb')

	result, buf = cv2.imencode('.png', rgb_crop)
	compr_stream = zlib.compress(buf,zlib.Z_BEST_COMPRESSION)
	print("[P] Compression done.")
	## Write to pipe part
	
	if (pred >= THRESHOLD):
		message = f'Aurora Detected with probability {100*pred[0][0]:.2f}%\0'
		pid.write(message.encode())
	
	else:
		message = b'[P] No Aurora was detected.\0'
		pid.write(message)
	print(f'[P] Message written to pipe, size = {len(message)} bytes')

	pid.close()
	# pid.close()
	run = False

