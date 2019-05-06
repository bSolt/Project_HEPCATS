# This will be a python module for collecting spacecraft data in 
#  cdf format and saving it to a single pickle file

import pickle
import os
import datetime
import argparse
import numpy as np
from spacepy import pycdf

DELAY = datetime.timedelta(minutes=30)

ap = argparse.ArgumentParser()
ap.add_argument('-d','--Directory',type=str,required=True,
	help='The name of the directory containing cdf files')

args = vars(ap.parse_args())

cdf_dir = args["Directory"]
if cdf_dir[-1] != '/':
	cdf_dir += '/'

cdf_list = os.listdir(cdf_dir)

data = {
'Image':np.array([]).reshape((0,256,256)),
'Epoch':np.array([]),
'Shuffled':False
}

def add_to_data(cdf,i):
	# print(f'Adding {i}')
	imdata = np.expand_dims(cdf[IM][i],0)
	epdata = np.expand_dims(cdf[EP][i],0)
	data['Image'] = np.append(data['Image'],imdata,0)
	data['Epoch'] = np.append(data['Epoch'],epdata,0)

for file in cdf_list:
	if '.cdf' in file:
		file = cdf_dir+file
		print(f'Accessing {file}')
		cdf = pycdf.CDF(file)

		if 'WIC_PIXELS' in cdf.keys():
			IM = 'WIC_PIXELS'
			EP = 'EPOCH'
		elif 'Image_Counts_Clean' in cdf.keys():
			IM = 'IImage_Counts'
			EP = 'Epoch'
		
		i = 0
		# iterate through cda
		while(i<len(cdf[EP])):
			add_to_data(cdf,i)
			t0 = cdf[EP][i]
			# skip all the images which are taken less than some time 
			#  (DELAY) later the previous
			while(cdf[EP][i]-t0 <DELAY):
				i+=1
				# print(i,end='\r')
				if(i>=len(cdf[EP])):
					# print('break57')
					break

# Save data with pickle
outname = cdf_dir.split('/')[1] + '.pickle'
with open(outname,'wb') as file:
	pickle.dump(data,file)

print(f'Data from cdfs in {cdf_dir} saved to '+
	  f'{outname}\nTotal images: {len(data["Image"])}')