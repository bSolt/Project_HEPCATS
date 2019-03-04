# saves labeled data to image files in a specified dir
# Inputs: .pickle files in the directory INDIR which are dict objects with 
# 	the field 'Image' as a key to a large array of single channel images
# Outputs: Seperate png files saved into the directory OUTDIR

# Author: Braden Solt
# Date Created: 2019-01-24

import os, pickle
import numpy as np
from PIL import Image 


INDIR = '../winter_data/pickle_v2/'
OUTDIR = '../winter_data/png_v2/'

SCALING = 'maximum'
# Other modes are 'normalize'

# what do the labels mean
sort = {'a': 'positives',
		'b': 'negatives',
		}

sums = {'a': 0,
		'b': 0}

for file in os.listdir(INDIR):
	# loop through the pickle files
	print(f'Processing {file}')
	with open(INDIR+file,'rb') as fid:
		data = pickle.load(fid)
	# do something for each label type
	# for label in sort.keys():
	# print(f'Saving to {sort[label]}')
	# ind = data['label']==label
	images = data['Image']
	# Image scaling
	if SCALING == 'maximum':
		# scale the images by their maximum s/t the new maximum is 256
		images *= 256/(images.max(axis=(1,2))).reshape((len(images),1,1))
		# take the floor and change dtype
		images = images.astype(np.uint8,copy=False)
	elif SCALING =='normalize':
		# remap the pixels to have mean 115 and std 60
		STD_SET = 60
		MEAN_SET = 115

		images -= images.mean(axis=(1,2))
		images *= STD_SET/images.std(axis=(1,2))
		images += MEAN_SET
		images = images.astype(np.uint8,copy=False)

	# Image Saving
	# create the output directories
	for label in sort.keys():
		location = OUTDIR+sort[label]
		if not os.path.isdir(location):
			os.mkdir(location)
		# keep track of how many of each image there is
		sums[label] += (data['label']==label).sum()
	for ii in range(len(images)):
		if data['label'][ii]=='c':
			continue
		location = OUTDIR+sort[data['label'][ii]]+'/'
		savename = location+'_'.join(file.split('_')[0:2])+f'_{ii:04d}.png'
		Image.fromarray(images[ii]).save(savename)
