# saves labeled data to image files in a specified dir

import os, pickle
import numpy as np
from PIL import Image 


INDIR = './winter_data/pickle/'
OUTDIR = './winter_data/png/'

# what do the labels mean
sort = {'a': 'positives',
		'b': 'negatives'}

for file in os.listdir(INDIR):
	# loop through the pickle files
	print(f'Loading {file}')
	with open(INDIR+file,'rb') as fid:
		data = pickle.load(fid)
	# do something for each label type
	for label in sort.keys():
		print(f'Saving to {sort[label]}')
		ind = data['label']==label
		images = data['Image'][ind]
		location = OUTDIR+sort[label]
		if ~os.path.isdir(location):
			os.mkdir(location)
		for ii in range(len(images)):
			savename = location+f'/{ii:04d}.png'
			Image.fromarray(images[ii]).save(savename)
