import tensorflow as tf
import numpy as np
from ips_helper import *

if __name__ == '__main__':
	# # Get the pre-trained deep neural network with --the correct input shape
	# inshape = (256,256,3)
	# ptdnn = get_xception(inshape)
	# # Build the classifier
	# classifier = build_classifier(ptdnn.output_shape)
	# # Build and compile the full model
	# model = build_full_model(ptdnn, classifier)

	model_file = '../models/fine3_300.h5'
	train_dir = '../OITL'
	epochs = 100

	model = tf.keras.models.load_model(model_file,
		custom_objects={'recall':recall,'f1':f1})

	# anonymous function for rotating 90 degrees randomly
	random_90 = lambda im: np.rot90(im,k=np.random.choice(4))
	#define the settings for loading in images including value rescale, 
	#  and random alterations such as scaling, zooming, and flipping
	augmented_gen = tf.keras.preprocessing.image.ImageDataGenerator(
		rescale=1./255,
		zoom_range=0.1,
		cval=0,
		horizontal_flip=True,
		vertical_flip = True,
		preprocessing_function=random_90,
	)

	training_gen = augmented_gen.flow_from_directory(
	    train_dir,
	    target_size=(256, 256),
	    batch_size=32,
	    class_mode='binary'
	)

	# Fit once more on augmented data
	h = model.fit_generator(
		training_gen,
		# validation_data=validation_gen,
		epochs=epochs
	)
	pres_plot.plot_history(
		h.history,
		color='white',
		title='Re-training on STK data',
		save='retrain_1.png'
	)