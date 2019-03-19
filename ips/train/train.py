# Program for training the neural network classifier


## Imports

import os, argparse
import numpy as np
import pres_plot
import tensorflow as tf
import matplotlib.pyplot as plt
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from ips_helper import *
# from PIL import Image
# import scipy.stats as stats

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
os.environ['TF_CPP_MIN_VLOG_LEVEL'] = '3'

# Check gpu setting
if tf.test.is_gpu_available():
  print(f'[TF] Running on GPU {tf.test.gpu_device_name()}')
else:
  print('[TF] Running on CPU')

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
def build_classifier(feature_shape):
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
  # for layer in feature_detector.layers:
  #   layer.trainable=False
  #   model.add(layer)
  #Then classify
  #model.add(classifier)  
  for layer in classifier.layers:
    model.add(layer)
  #feature_detector should not be trainable if fine_tuning_layers==0
  feature_detector.trainable=False

  # Display layer information for reference
  print('[ASSEMBLE] Full Model Architecture:')
  model.summary()

  # classifier options including metrics and loss function
  classifier.compile(optimizer=tf.keras.optimizers.RMSprop(lr=2e-4),
                loss='binary_crossentropy',
                metrics=['acc',recall,f1])
  model.compile(optimizer=tf.keras.optimizers.RMSprop(lr=2e-5),
                loss='binary_crossentropy',
                metrics=['acc',recall,f1])

  print('[ASSEMBLE] Models compiled successfully')
  return model

"""
This function will enable fine tuning on the ptdnn
"""
def enable_fine_tuning(ptdnn,fine_layer):
  # make the base network trainable
  ptdnn.trainable = True

  set_trainable = False
  for layer in ptdnn.layers:
      if layer.name == fine_layer:
          set_trainable = True
      if set_trainable:
          layer.trainable = True
      else:
          layer.trainable = False
  # function does not need to return an object b/c the model is mutable

# valid fine tuning layers for now
valid_x_fine = ['block14_sepconv2','block14_sepconv1']

"""This fucntion  will load in the data and organize it into batches
The next block generates batches of training data. The feature array output by the 
feature detector network is of size 8 by 8 by 2048 for each image as an example. 
"""
def compute_features_from_dir(directory,feature_detector,
  batch_size=32,save_to_dir=None):
  # Detect and load all images from a directory, use the feature detector network which
  # is passed in to compute the features for each image
  #  The directory argument should be a valid directory containing a directory for each class
  #   in our case, it should contain postives/ and negatives/

  # Define total number of images
  N_im = sum([len(files) for r, d, files in os.walk(directory)])
  # Get shape of feature vector from feature_detector network

  datagen = ImageDataGenerator(rescale=1./255)
  # Define the settings for assembling the batches
  #  including image size, directory,  and batch size
  if save_to_dir and not os.path.isdir(save_to_dir):
    os.mkdir(save_to_dir)

  image_generator = datagen.flow_from_directory(
    directory,
    target_size=(256, 256),
    batch_size=batch_size,
    class_mode='binary',
    save_to_dir=save_to_dir)
  # pre-allocate features array

  feature_shape = feature_detector.output_shape 
  featrs = np.zeros(
    shape=np.concatenate(([N_im], feature_shape[1:]))
    )
  # pre-allocate labels array
  labels = np.zeros(N_im)
  # Iterate over batches and compute the features
  i=0 # batch index
  total_batch = N_im//batch_size 
  for images_batch, labels_batch in image_generator:
    print(f"[COMPUTE_FEATURES] Computing Features and Labels for image batch {i}/{total_batch}",end='\r')
    # This is the line that finds the features using the ptdnn
    features_batch = feature_detector.predict(images_batch)
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

def train_repeatedly(M, classifier, epochs, features, labels, v_split = 0.3):
  results = {}
  for i in range(M):
    print(f'[TRAIN] Randomizing Model and Data {i+1}/{M}', end='\r')
    # Reset the weights
    randomize_weights(classifier)
    # Shuffle the features
    order = np.random.permutation(len(labels))
    features = features[order]
    labels   = labels[order]
    # print statement
    print(f'[TRAIN] Training Model Simulation  {i+1}/{M}',end='\r')
    # Train
    history = classifier.fit(
      features, labels,
      validation_split = v_split,
      epochs=epochs,
      verbose=False)
    # store each result
    # First iterate on the keys present
    for key in history.history.keys():
      # We will have to initialize each key as an empty list in order to get
      # A list of lists as the object which is stored by the dict.
      if key not in results.keys():
        results[key] = []
      # add the current history into results as an item of a list with .append()
      results[key].append(history.history[key])
  print() #newline
  return results


def main():
  # defined directory for images
  default_dir = '../winter_data/png_v3/'
  # Options for parsing the command line arguments
  ap = argparse.ArgumentParser()
  ap.add_argument("-e", "--epochs", type=int, default=30,
  help="nuber of epochs to train the classifier network on")
  ap.add_argument("-m", "--simulationnumber", type=int, default=1,
  help="number of simulations to use for training the classifier")
  ap.add_argument("-f", "--finetuning", type=int, default=0,
  help="An optional argument for how much fine-tuning to apply")
  ap.add_argument("-s", "--saveas", type=str, default=None,
  help="Name to pass for saving the trained model to an h5 file")
  ap.add_argument("-id", "--imagedir", type=str, default=default_dir,
  help="Directory where training images are located")
  ap.add_argument("-fs", "--featureset", type=str, default='feature_set/',
  help="Directory where the feature set files should be saved and loaded from")
  ap.add_argument("-c", "--cache", type=str, default=None,
  help="Name to pass for saving the images which the network uses")
  # ap.add_argument("-p", "--plotting", action='store_const',
  # const=True,default=False,
  # help="flag for generating the stats plots, altrernative to pres_plot.f1_plot()")
  args = vars(ap.parse_args())
  # defined directory for images
  image_dir = args['imagedir']
  # Get the pre-trained deep neural network with --the correct input shape
  inshape = (256,256,3)
  ptdnn = get_xception(inshape)
  # Build the classifier
  classifier = build_classifier(ptdnn.output_shape)
  # This should be 1 or 2
  ft_option = args['finetuning']

  # If it's allowed, remake models to account for fine-tuning
  possible = (3,19,22,29)

  # Pretty sure this is the wrong approach...
  # if ft_layers:
  #   if ft_layers in possible:
  #     ft_layer = ptdnn.layers[-1-ft_layers]
  #     # next_layer = ptdnn.layers[-ft_layers]
  #     new_ptdnn = tf.keras.Model(
  #       name = 'frozen_ptdnn',
  #       inputs=ptdnn.input,
  #       outputs=ft_layer.output)

  #     new_classifier = tf.keras.models.Sequential(name='extended_classifier')
  #     new_classifier.add(tf.keras.layers.InputLayer(input_shape=ft_layer.output_shape[1:]))
  #     for l in ptdnn.layers[-ft_layers:]:
  #       new_classifier.add(l)
  #     new_classifier.add(classifier)
  #     new_classifier.build(new_ptdnn.output_shape)

  #     ptdnn = new_ptdnn
  #     # ptdnn.summary()
  #     print('[MAIN] Classifier + Fine-Tuning Layers')
  #     classifier = new_classifier
  #     classifier.summary()
  #   else:
  #     print('Invalid fine-tuning option!')
  #     exit -1

  # Build and compile the full model
  model = build_full_model(ptdnn, classifier)

  # Get the features if necessary
  #  Define the directory where this is stored
  if args['featureset'][-1]=='/':
    feature_dir = args['featureset']
  else:
    feature_dir = args['featureset'] + '/'
  print(f'[MAIN] Using directory ./{feature_dir} for feature set')
  # Define the path of files containing features and labels
  if not os.path.isdir(feature_dir):
    os.mkdir(feature_dir)
  feature_file = feature_dir + 'featrs.npy'
  label_file   = feature_dir + 'labels.npy'
  # Check if computation of features is needed
  if not os.path.isfile(feature_file):
    # Compute the features
    print(f'[MAIN] Computing new feature set in {feature_dir}')
    featrs, labels = compute_features_from_dir(image_dir,ptdnn,
      save_to_dir=feature_dir+args['cache'] if args['cache'] else None)
    # Save the arrays to the file
    print(f'[MAIN] Saving feature set')
    np.save(feature_file, featrs)
    np.save(label_file, labels)
  else:
    print('[MAIN] Loading in feature set')
    # Load the arrays from the files
    featrs = np.load(feature_file)
    labels = np.load(label_file)

  M =  args['simulationnumber']
  #define number of epochs
  epochs_0 = 30
  epochs = args['epochs']
  #Determine name for figure image
  psname = f'plot_e{epochs}_m{M}.png'
  fi=0
  while os.path.isfile(psname):
    psname = f'plot_e{epochs}_m{M}_{fi}.png'
    fi+=1
  print(f'[MAIN] Using {psname} for plot save location')

  ### Training Phase 1 ###
  #  In this phase of training the randomly initialized classifier net will be trained 
  #  on 30 epochs of unaltered features

  print('[MAIN] Training classifier once on saved features')
  h0 = classifier.fit(
    featrs, labels,
    validation_split=0.3,
    epochs=epochs_0
    )
  pres_plot.plot_history(h0.history,
    color='white',
    title='Initial Classifier Training',
    save='phase0.png')

  ### Training Phase 2 ###
  # In this phase of training, the network will be trained on augmented data
  # Optionally, fine-tuning may be applied

  # anonymous function for rotating 90 degrees randomly
  random_90 = lambda im: np.rot90(im,k=np.random.choice(4))
  #define the settings for loading in images including value rescale, 
  #  and random alterations such as scaling, zooming, and flipping
  augmented_gen = ImageDataGenerator(
    rescale=1./255,
    zoom_range=0.1,
    cval=0,
    horizontal_flip=True,
    vertical_flip = True,
    preprocessing_function=random_90,
    )
  # A generator without augmentation
  unaltered_gen = ImageDataGenerator(
    rescale=1./255,
    )
  # Tell the generator where to find the data and what size to load it as

  # Source of images
  train_dir = os.path.join(image_dir,'training')
  valid_dir = os.path.join(image_dir,'validation')
  # Check that the directories exist
  if not os.path.isdir(train_dir) or not os.path.isdir(valid_dir):
    print('[MAIN] Training or Validation directory not found')
    return  -1

  training_gen = augmented_gen.flow_from_directory(
    train_dir,
    target_size=(256, 256),
    batch_size=32,
    class_mode='binary')

  validation_gen = unaltered_gen.flow_from_directory(
    valid_dir,
    target_size=(256,256),
    batch_size=32,
    class_mode='binary')

  ### Training Phase 3 ###
  # In this phase, (fine tuning and) data augmentation is done
  if ft_option:
    print(f'[MAIN] Training full model with fine-tuning option {ft_option}')
    # enable fine-tuning of ptdnn
    enable_fine_tuning(ptdnn,valid_x_fine[ft_option-1])
    # Fit once more on augmented data
    h2 = model.fit_generator(
      training_gen,
      validation_data=validation_gen,
      epochs=epochs
      )
    return vars()
    pres_plot.plot_history(h2.history,
      color='white',
      title='Training on Augmented Data with augmentation',
      save='phase1.png')
  else:
    print('[MAIN] Training classifier on augmented data w/o fine-tuning')
    return vars()
    h1 = model.fit_generator(
      training_gen,
      validation_data=validation_gen,
      epochs=epochs
      )
    return vars()
    pres_plot.plot_history(h1.history,
      color='white',
      title='Training on Augmented Data without Adaptation',
      save='phase2.png')

  # Save the model if applicable    
  if args['saveas']:
    model.save('../models/' + args['saveas'] + '.h5')
  # return vars() s/t you can use the local variables here in interactive mode
  return vars()


if __name__ == '__main__':
  v = main()
  globals().update(v)


  ### Old training code
  # if M==1:
  #   print(f'[MAIN] Training Model Once...')
  #   history = classifier.fit(
  #           featrs, labels,
  #           validation_split = 0.3,
  #           epochs=epochs)
  #   pres_plot.plot_history(history.history,
  #     save=psname,color='white',
  #     title=f'Accuracy metrics for training on {feature_dir} feature set')
  #   # pres_plot.plot_errors(classifier,labels,)
  # # Case for training multiple times
  # else:
  #   print(f'[MAIN] Training Model {M} times...')
  #   # Call function which trains model multiple times
  #   results = train_repeatedly(M, classifier, epochs, featrs, labels)
  #   # Call function which plots results from this test
  #   pres_plot.f1_plot(results,
  #     title=f"Validation for training {M} times on {feature_dir.replace('_',' ')[0:-1]}",
  #     save =psname,
  #     color='white')
  
