# Program for training the neural network classifier


## Imports

import os, argparse
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from ips_helper import *
# from PIL import Image
# import scipy.stats as stats


## Set up the feature detector
"""
# Pre-Trained Network Setup & Feature Extraction
The next code block downloads the Pre-trained network configuration and weights.  
Luckily the chosen network is included in keras, so this process if fairly straightforward. 
"""
def get_xception(inshape):
  # inshape = (256,256,3)
  ptdnn = tf.keras.applications.Xception(
  	weights='imagenet',
  	include_top=False,
  	input_shape=inshape)
  return ptdnn

## Set up the classifier
"""
# Building the Classifier Network
Next we must implement our own classifier network to be trained on the data. 
Luckily, using Keras functions, this processes is fairly straightforward.

The next code block sets up and trains the classifier network. This simple classifier 
network only have input, hidden layer, dropout, and output layers. The input layer 
corresponds to the the feature array generated by the feature detector network. 
These features are used to determine the values of the hidden layer which has 256 neurons in one dimension. 
The following layer to this is a dropout layer. this dropout unit randomly ignores half of the values from 
the hidden layer. This is useful for preventing overfitting. The output layer is just a single neuron with a 
sigmoid activation. A sigmoid activation is used here because its output can be interpreted as a probability
that the image is a positive case.

Other options specified here are in the compile method of the code. First, the model was copiled and several 
options were chosen. The chosen optimizer is known as RMSprop which is known to have very good learning 
characteristics. The `lr` variabile passed to this stands for learning rate and simply paramaterizes how 
quickly the network attempts to change weight values by during training. The loss function was specified 
as a binary, cross-entropy loss becase this is also known to yield good learning values. Metrics that will 
be recorded are specified as accuracy, recall, and F1 score.
In the last line, training settings defined above are passed to the `fit()` method. Namely a batch size of 
32 images and a total epoch count which is specified by the command line argument.
"""
def build_classier():
  classifier = tf.keras.models.Sequential(name='Classifier')
  # The classifier input is dense, or fully connected
  classifier.add(tf.keras.layers.Dense(256, activation='relu',
    input_shape=feature_shape[1:]))
  # The flatten layer reduces the dimensions of the output
  classifier.add(tf.keras.layers.Flatten())
  # Dropout layer prevents overfitting
  classifier.add(tf.keras.layers.Dropout(0.5))
  # Output layer is a single neuron sigmoid
  classifier.add(tf.keras.layers.Dense(1, activation='sigmoid'))
  return classifier
"""
This model will implement the combined model which detects image features 
and classifies the images in one pass. This will be the model which is actually
used for the aurora identification
"""
def build_full_model(feature_detector, classifier):
  model = tf.keras.models.Sequential()
  #First get the features from ptdnn
  model.add(feature_detector)
  #Then classify
  model.add(classifier)
  #ptdnn should not be trainable for now
  model.layers[0].trainable=False
  # Display layer information for reference
  print('[INFO] Full Model Architecture:')
  model.summary()

  # classifier options including metrics and loss function
  classifier.compile(optimizer=tf.keras.optimizers.RMSprop(lr=2e-4),
                loss='binary_crossentropy',
                metrics=['acc',recall,f1])
  model.compile(optimizer=tf.keras.optimizers.RMSprop(lr=2e-4),
                loss='binary_crossentropy')

  print('[INFO] Models compiled successfully')
  return model
"""This fucntion  will load in the data and organize it into batches
The next block generates batches of training data. The feature array output by the 
feature detector network is of size 8 by 8 by 2048 for each image as an example. 
"""
def compute_features_from_dir(directory,feature_detector,
  batch_size=32):
  # Detect and load all images from a directory, use the feature detector network which
  # is passed in to compute the features for each image
  #  The directory argument should be a valid directory containing a directory for each class
  #   in our case, it should contain postives/ and negatives/

  # Define total number of images
  N_im = sum([len(files) for r, d, files in os.walk(directory)])
  # Get shape of feature vector from feature_detector network
  feature_shape = feature_detector.output_shape

  #define the settings for loading in images including value rescale, 
  #  and random alterations such as scaling, zooming, and flipping
  datagen = ImageDataGenerator(
    rescale=1./255,
    zoom_range=0.1,
    horizontal_flip=True,
    vertical_flip = True,
    brightness_range=[1/2,2.0])
  # Define the settings for assembling the batches
  #  including image size, directory,  and batch size
  image_generator = datagen.flow_from_directory(
    directory,
    target_size=(256, 256),
    batch_size=batch_size,
    class_mode='binary')
  # pre-allocate features array 
  featrs = np.zeros(
    shape=np.concatenate(([N_im], feature_shape[1:]))
    )
  # pre-allocate labels array
  labels = np.zeros(N_im)
  # Iterate over batches and compute the features
  i=0 # batch index
  total_batch = N_im//batch_size 
  for images_batch, labels_batch in image_generator:
    print(f"[INFO] Computing Features and Labels for image batch {i}/{total_batch}",end='\r')
    # This is the line that finds the features using the ptdnn
    features_batch = ptdnn.predict(images_batch)
    #Case for most batches
    if ((i+1) * batch_size < N_im):
      featrs[i * batch_size : (i + 1) * batch_size] = features_batch
      labels[i * batch_size : (i + 1) * batch_size] = labels_batch
    #Case for last batch
    else:
      featrs[i * batch_size : ] = features_batch[0:N_im-i*batch_size]
      labels[i * batch_size : ] = labels_batch[0:N_im-i*batch_size]
    i += 1
    #break when done
    if i * batch_size >= N_im:
        break
  # Return the completed arrays
  return featrs, labels


def main():

  image_dir = '../winter_data/png_v2/'
  # Get the pre-trained deep neural network with the correct input shape
  inshape = (256,256,3)
  ptdnn = get_xception(inshape)
  # Build the classifier
  classifier = build_classier()
  # Build and compile the full model
  model = build_full_model()
  # Get the features if necessary
  #  Define the files where this will is stored
  feature_file = 'feature_set/featrs.npy'
  label_file   = 'feature_set/lables.npy'
  # Check if computation of features is needed
  if not os.path.isfile(feature_file):
    # Compute the features
    featrs, labels = compute_features_from_dir(image_dir)
    # Save the arrays to the files
    np.save(feature_file, featrs)
    np.save(label_file, labels)
  else
    # Load the arrays from the files
    featrs = np.load(feature_file)
    labels = np.load(label_file)

  # Train the network

  #define number of epochs
  epochs = 50

  history = classifier.fit(
          featrs, labels,
          validation_split = 0.3,
          epochs=epochs)

if __name__ == '__main__':
  main()