import sys
import numpy
import zlib
import glob
from PIL import Image

image_list = []

for filename in glob.glob('uncompressed_images/*'):
	fname = filename[20:]
	print '\nCOMPRESSING FILE:    '+fname
	im = open(filename,'rb').read()
	image_list.append(im)
	comp_im = zlib.compress(im, zlib.Z_BEST_COMPRESSION)
	comp_ratio = float(len(im)) / float(len(comp_im))
	with open('compressed_images/compressed_'+fname,'wb') as f:
		f.write(comp_im)
	print 'COMPRESSION RATIO:  ',comp_ratio
	print "COMPRESSED %d%%" % (len(comp_im) * 100 / len(im))
	print fname, len(im), "=>", 'compressed_'+fname ,len(comp_im)

	# Decompress the images @ groundstation
	comp4final_im = open('compressed_images/compressed_'+fname,'rb').read()
	final_im = zlib.decompress(comp4final_im)
	with open('final_images/final_'+fname,'wb') as fin:
		fin.write(final_im)
	print 'DECOMPRESSING:' 
	print 'final image =>', 'final_'+fname, len(final_im)

