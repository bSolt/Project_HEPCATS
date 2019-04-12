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
import numpy as np
import cv2, os

def read_raw(pipe, read_size):
	# Expected size of image
	row = 1920; col = 1200;
	# create np array from bufer with type uint8 and reshape to correct size
	raw_arr = np.frombuffer(
		os.read(pipe, read_size)
		,np.uint8).reshape((col,row))
	# De-mosaic using openCV
	rgb_arr = cv2.cvtColor(raw_arr, cv2.COLOR_BayerRG2RGB)
	return rgb_arr

# MAIN FUNCTION
if(__name__=='__main__'):
	import tensorflow as tf
	import zlib, argparse
	# This one is a custom buffer reading object
	# from FixedBufferReader import FixedBufferReader
	# This one is the cropping function based on circle segmentation
	from croppingScript import auto_crop
	from ips_helper import recall, f1, fix_colors

	ap = argparse.ArgumentParser()
	ap.add_argument("-p","--pipe", type=str, default="/dev/rtp0",
	  help="name of pipe (fifo) to use")
	ap.add_argument("-t","--image_type", type=str, default="ieu",
	  help="string for method of reading image: test for using rawpy or ieu for direct array file")
	ap.add_argument("-m","--model", type=str, default = "../models/fine3_300.h5",
		help = "The path the the model file to load in at the beginning of the script")
	ap.add_argument("-k","--keep_color", action='store_true', default=False, 
		help = "whether or not to use RGB color when classifying the image")
	ap.add_argument("-v","--verbose", action='store_true', default=False, 
		help = "whether or not to print verbose statements including timings")

	args = vars(ap.parse_args())
	# IMAGE_FORMAT = "test"
	# Use this option for IEU integration
	IMAGE_FORMAT = args['image_type']
	# Name of pipe to be used
	COMM_PIPE = args['pipe']

	#import time if necessary
	if args['verbose']:
		import time, datetime
		if args['keep_color']:
			print('[P] Keeping color for classification')
		print('[P] Using model file: {}'.format(args['model']))

	# This will be the file containing the full neural network model
	MODEL_FILE = args['model']

	# Threshold for Aurora Detection
	THRESHOLD = 0.5

	# Read the neural network model from a file
	model = tf.keras.models.load_model(MODEL_FILE,
		custom_objects={'recall':recall,'f1':f1})

	if args['verbose']:
		print('[P] Model file loaded successfully!')

	# All supported image sizes
	image_size = {
	'test':9861950,
	'ieu' :2304000,
	'default':2304000
	}
	# assign the number of bytes to be read in
	if IMAGE_FORMAT in image_size.keys():
		BYTES = image_size[IMAGE_FORMAT]
	else:
		BYTES = image_size['default']

	# Open the pipe for reading and writing using os module
	pipe = os.open(COMM_PIPE, os.O_RDWR)
	# Send the message that ips is ready to begin processing
	ready_message = np.uint8(21)
	os.write(pipe, ready_message)

	if args['verbose']:
		print("Ready message sent. Expecting to read {} bytes".format(BYTES))
	#The program will loop while run is True
	# It is not designed to stop in its current state
	run = True

	# Infinite Loop
	while(run):
		# verbose message indicating that loop has been entered
		if args['verbose']:
			print("[P] Reading from {}".format(COMM_PIPE))
		# Read in image
		if ( IMAGE_FORMAT=='test' ):
			raw = rawpy.imread(pipe)
			# Convert the raw image to a uint8 numpy array
			rgb_arr = raw.postprocess(gamma=(1,1))
		else:
			# use read_raw function
			rgb_arr = read_raw(pipe,BYTES)
		# Announce cropping and start cropping timer
		if args['verbose']:
			print("[P] Now cropping... Cross your fingers")
			t0 = time.time()
		# call cropping function
		rgb_crop, pcode, ecode = auto_crop(rgb_arr,minAccumulatorVotes=100)
		if args['verbose']:
			dt = datetime.timedelta(seconds=time.time()-t0)
			print("[P] Crop done: pcode = {} ecode = {}".format(pcode, ecode))
			print("[P] Cropping time: {}".format(dt))
		# Stop the program if there's a cropping error.
		if (pcode!=0 or ecode!=0):
			print("[P] Cropping Error")
			# set the output image to the full image for saving purposes
			rgb_crop = rgb_arr
			# Set pred to 0 so that it is equivalent to aurora not found
			pred=0
		# Detect aurora after cropping
		else:
			# resizing is done using the opencv function
			rgb_small = cv2.resize(rgb_crop, (256,256))
			# create the input for the NN
			if args['keep_color']: #classify the input image with color
				classify = np.expand_dims(rgb_small,0)
			else: #remove color from the image
				# Convert to grayscale for classifying
				gray_small = cv2.cvtColor(rgb_small,cv2.COLOR_RGB2GRAY)
				# expand dims to prepare for input to neural net
				classify = np.expand_dims(fix_colors(gray_small),0)
			
			if args['verbose']:
				print("[P] Now classifying image")
				t0 = time.time()
			# apply neural net model
			pred = model.predict(classify)
			if args['verbose']:
				dt = datetime.timedelta(seconds=time.time()-t0)
				print('[P] {:.2f}% chance of Aurora detected in image'.format(100*pred[0][0]))
				print('[P] Classify time: {}'.format(dt))
				# Save gray image
				gresult, gbuf = cv2.imencode('.png', classify[0])
				with open('input.png','wb') as file:
					file.write(gbuf)
		# Convert cropped image to png buffer for downlink
		result, buf = cv2.imencode('.png', rgb_crop)
		do_label = False
		if do_label:
			labeled = rgb_crop.copy()
			label = 'Aurora ' + ('not' if pred<=THRESHOLD else '') + ' present'
			cv2.putText(labeled, "{}, {:.2f}%".format(label, pred[0][0] * 100),
				(10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)
			result, buf_l = cv2.imencode('.png', labeled)
		else:
			buf_l = buf
		if args['verbose']:
			print('[P] PNG buffer created with result {}'.format(result))
			
		# Check if the auroral threshold is met or not
		if (pred > THRESHOLD):
			# MATT COMPRESSION
			# Current strategy: Encode image to png, then apply zlib
			compr_stream = zlib.compress(buf,zlib.Z_BEST_COMPRESSION)
			if args['verbose']:
				print("[P] Image compressed to size {}\ttotal ratio = {}".format(\
					len(compr_stream),len(compr_stream)/BYTES))
				# Save the image to local
				sind = 0
				sname = "../positives/{0:05d}.png".format(sind)
				# get a unique savename
				while os.path.isfile(sname):
					sind += 1
					sname = "../positives/{0:05d}.png".format(sind)

				print("[P] Saving image to {}".format(sname))
				# Do the saving
				with open(sname,'wb') as file:
					# result, buf = cv2.imencode('.png', rgb_crop) see line 153
					file.write(buf_l)				
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
			if args['verbose']:
				print('[P] compressed PNG written with size = {} bytes'.format(len(compr_stream)))
		else:
			# If no aurora is detected, we simply write EOF (0x00) to the pipe instead
			os.write(pipe,np.uint32(0))
			if args['verbose']:
				print('[P] EOF written to pipe')

				# Save the image to local
				sind = 0
				sname = "../negatives/{0:05d}.png".format(sind)
				while os.path.isfile(sname):
					sind += 1
					sname = "../negatives/{0:05d}.png".format(sind)
				print("[P] Saving image to {}".format(sname))
				# Do the saving
				with open(sname,'wb') as file:
					file.write(buf_l)
