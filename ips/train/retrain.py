import tensorflow as tf
import numpy as np
import argparse
from ips_helper import *
from train import enable_fine_tuning

if __name__ == '__main__':
	# # Get the pre-trained deep neural network with --the correct input shape
	# inshape = (256,256,3)
	# ptdnn = get_xception(inshape)
	# # Build the classifier
	# classifier = build_classifier(ptdnn.output_shape)
	# # Build and compile the full model
	# model = build_full_model(ptdnn, classifier)

	ap = argparse.ArgumentParser()
	ap.add_argument("-e", "--epochs", type=int, default=100,
		help="Number of epochs to use in the main training loop")
	ap.add_argument("-td", "--train_dir", type=str, default = "../stk_all",
		help="Name of the directory which contains data to retrain the network on")
	ap.add_argument("-lr", "--learn_rate", type=float, default=1e-4,
		help="Learning rate to use for the classifier retraining")
	args = vars(ap.parse_args())

	model_file = '../models/fine3_300.h5'
	train_dir = args["train_dir"]
	valid_dir = '../combined/validation/'
	epochs = args['epochs']

	model = tf.keras.models.load_model(model_file,
		custom_objects={'recall':recall,'f1':f1})
	# Change fine-tuning to "option 1"
	enable_fine_tuning(model.get_layer('xception'),'block13_sepconv1_act')

	#recompile
	model.compile(
		optimizer=tf.keras.optimizers.RMSprop(lr=1e-5),
        loss='binary_crossentropy',
        metrics=['acc',recall,f1]
    )

	# anonymous function for rotating 90 degrees randomly
	random_90 = lambda im: np.rot90(im,k=np.random.choice(4))
	#define the settings for loading in images including value rescale, 
	#  and random alterations such as scaling, zooming, and flipping
	augmented_gen = tf.keras.preprocessing.image.ImageDataGenerator(
		rescale=1./255,
		zoom_range=0.1,
		rotation_range = 180,
		cval=0,
		horizontal_flip=True,
		vertical_flip = True,
		width_shift_range=0.2,
		height_shift_range=0.2,
		brightness_range=(0.5,1.5)
		# preprocessing_function=random_90,
	)

	datagen = tf.keras.preprocessing.image.ImageDataGenerator(
		rescale=1./255,
	)

	training_gen = augmented_gen.flow_from_directory(
	    train_dir,
	    target_size=(256, 256),
	    batch_size=32,
	    class_mode='binary'
	)

	validation_gen = datagen.flow_from_directory(
		train_dir,
	    target_size=(256, 256),
	    batch_size=32,
	    class_mode='binary'
	)

	# Fit once more on augmented data
	h = model.fit_generator(
		training_gen,
		validation_data=validation_gen,
		epochs=epochs
	)
	# pres_plot.plot_history(
	# 	h.history,
	# 	color='white',
	# 	title='Re-training on STK data',
	# 	save='retrain_1.png'
	# )
	mname = '../models/retrained.h5'
	ii=0
	while os.path.isfile(mname):
		mname = '../models/retrained_{}.h5'.format(ii)
	model.save(mname)