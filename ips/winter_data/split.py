# This script will split file into two subsets
# some proportion, p of the data will be copied to a validation folder
# the remaining image will be compied to a training folder

import argparse, os
from shutil import copyfile
from math import floor
from random import shuffle

# Setup arguments for script
ap = argparse.ArgumentParser()
ap.add_argument('original', type=str,
	help="The original directory of the data containing subfolders for each class")
ap.add_argument('new', type=str,
	help="The new directory of the data containing subfolders for training/validation")
ap.add_argument('-p','--probability',default=0.3,
	help="The proportion of images to be set aside for validation")

args = vars(ap.parse_args())

# Make the source directory if needed
# if not os.path.isdir(args["new"]):
# 	print(f'Making {args["new"]}')
# 	os.mkdir(args["new"])

# Iterate over classes
for class_dir in  os.listdir(args["original"]):
	print(f'Copying files for class {class_dir}')
	# original class path
	c_path = os.path.join(args["original"], class_dir)
	# Define and make new training dir
	train_dir = os.path.join(args["new"],'training',class_dir)
	if not os.path.isdir(train_dir):
		print(f'Making {train_dir}')
		os.makedirs(train_dir)
	# Define and make new validation dir
	valid_dir = os.path.join(args["new"],'validation',class_dir)
	if not os.path.isdir(valid_dir):
		print(f'Making {valid_dir}')
		os.makedirs(valid_dir)
	# The names of the files in this class
	files = [n for n in os.listdir(c_path) if os.path.isfile(os.path.join(c_path,n))]
	shuffle(files)
	# Number of files per class
	n_orig = len(files)
	# Number of files for training
	n_train = floor(n_orig*(1-args["probability"]))
	print(f'Copying {n_train} Training Files')
	for file in files[:n_train]:
		copyfile(os.path.join(c_path,file), os.path.join(train_dir,file))
	print(f'Copying {n_orig-n_train} Validation Files')
	for file in files[n_train:]:
		copyfile(os.path.join(c_path,file), os.path.join(valid_dir,file))