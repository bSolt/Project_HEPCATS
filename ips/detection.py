# This script is responsible for loading and using the neural network model

import tensorflow as tf
import numpy as np
import os, rawpy, zlib
from PIL import Image
#from ips_helper import *

MODEL_FILE = "winter_model_1.h5"
# INPUT_PIPE = "/dev/rtp0"
INPUT_PIPE = "IMG_6591.CR2"
THRESHOLD = 0.5
OUT_TEST = "output.npy"

model = tf.keras.models.load_model(MODEL_FILE)

#The probrom will loop while run is True
run = True
# Open the input pipe for reading and writing
# pipe = open(INPUT_PIPE,'r+b')

def auto_crop(image):
	return croppped_image, error_code

while(run):
	# raw = read_from_pipe(INPUT_PIPE)

	# PIPES
	# Find out if this works and is blocking
	raw = rawpy.imread(INPUT_PIPE)
	rgb_full = raw.postprocess()
	# KIAN'S FUNCTION
	# concerned about run time, might need to fix later
	# in the future we might 
	rgb_crop, error_code = auto_crop(rgb_full)

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

	print(f'[INFO] {INPUT_PIPE} has rated as {pred}')

	## Write to pipe part
	with open(OUT_TEST,'wb') as pipe:
		if (pred >= THRESHOLD):
			np.save(pipe,rgb)
			# pipe.write(compr_stream)
			print(f'[INFO] rgb array saved to {OUT_TEST}. Array shape: {rgb.shape}')
		else:
			pipe.seek(999999999)
			pipe.write('\0')

	run = False

