#!/usr/bin/python3
# This will be a script for classifying images

from __future__ import absolute_import, division, print_function
import sys
from keras.models import load_model
from keras.applications.inception_resnet_v2 import InceptionResNetV2
import tensorflow as tf
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

# tf.enable_eager_execution()
#Do some metrics huh
from keras import backend as K

def recall(y_true, y_pred):
    """Recall metric.

    Only computes a batch-wise average of recall.

    Computes the recall, a metric for multi-label classification of
    how many relevant items are selected.
    """
    true_positives = K.sum(K.round(K.clip(y_true * y_pred, 0, 1)))
    possible_positives = K.sum(K.round(K.clip(y_true, 0, 1)))
    recall = true_positives / (possible_positives + K.epsilon())
    return recall

def precision(y_true, y_pred):
    """Precision metric.

    Only computes a batch-wise average of precision.

    Computes the precision, a metric for multi-label classification of
    how many selected items are relevant.
    """
    true_positives = K.sum(K.round(K.clip(y_true * y_pred, 0, 1)))
    predicted_positives = K.sum(K.round(K.clip(y_pred, 0, 1)))
    precision = true_positives / (predicted_positives + K.epsilon())
    return precision

def f1(y_true, y_pred):
    """ F1 metric.
    The F1 metric is the marmonic mean of precision and recall
    """
    p = precision(y_true, y_pred)
    r = recall(y_true, y_pred)
    return 2*((p*r)/(p+r+K.epsilon()))

def classify_image(image_data,ptdnn,classifier):
  n = image_data.shape[0]
  im_mat = np.asarray(image_data).reshape(n,256,256,3)
  features = ptdnn.predict(im_mat).reshape(n,6*6*1536)
  return classifier.predict(features)

if (__name__=='__main__'):
	if(len(sys.argv)<3):
		print('Script requires 2 input arguments: classify_script [model h5 file] [image file]')
		sys.exit(1)
	modelfile = sys.argv[1]
	imgfile   = sys.argv[2]
	print('Using file', modelfile, 'for classifier model')
	print('Using file', imgfile, 'for image to classify')

	ptdnn = InceptionResNetV2(weights='imagenet',
		include_top=False,
		input_shape=(256,256,3))

	# ptdnn.summary()
	print('PTDNN loaded')

	model = load_model(modelfile,
		custom_objects={'recall': recall, 
						'f1' : f1})

	print('Classifier Network Loaded')
	model.summary()

	# Image stuff
	im = Image.open(imgfile).resize((256,256)).convert("RGB")
	im = np.array(im).reshape(1,256,256,3)
	prediction = classify_image(im,ptdnn,model)

	print("Prediction:{}".format(prediction))
	fig,ax = plt.subplots(1,1,figsize=(12,6))
	plt.imshow(im[0])
	plt.grid(False)
	plt.show()
