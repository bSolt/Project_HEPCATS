# This script is responsible for loading and using the neural network model

import tensorflow as tf
import numpy as np
import os, rawpy
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

while(run):
	# raw = pipe.read()
	raw = rawpy.imread(INPUT_PIPE)
	# I hope that the above is blocking, but not sure exactly
	rgb = raw.postprocess()
	# imar = auto_crop(rgb)
	rgb_small = np.array(
		Image.fromarray(rgb).resize((256,256))
		)
	rgb_small = np.expand_dims(rgb_small,0)
	pred = model.predict(rgb_small)
	# input has to be read-only, tests use
	# compr_stream = compress_all(rgb,zlib.Z_BEST_COMPRESSION)

	print(f'[INFO] {INPUT_PIPE} has rated as {pred}')

	with open(OUT_TEST,'wb') as pipe:
		if (pred >= THRESHOLD):
			np.save(pipe,rgb)
			# pipe.write(compr_stream)
			print(f'[INFO] rgb array saved to {OUT_TEST}. Array shape: {rgb.shape}')
		else:
			pipe.seek(999999999)
			pipe.write('\0')

	run = False

