"""
transfer_stats.py is a script for determining how often a particular transfer learning model meets 
the requirement of f1>0.95. This is a proportion, so we can build an empirical model for estimating 
this. The estimator of the number of models which meet the requirement over the number tested can be
used to quantify how well a particular model works. This is done for each epoch count up to the value 
[-e EPOCHS] from the command line input

Usage: python3 [-i] transfer_demo.py [-h] [-model MODEL] [-s SAVEAS] [-e EPOCHS] [-p] [-t]"""

import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from PIL import Image
import tensorflow as tf
import argparse
from tensorflow.keras.preprocessing.image import ImageDataGenerator
import tensorflow.keras.backend as K
from scipy import stats as stats

# The default ptdnn to be used if a different one is not specified
default_net = "xception"
default_train = "polar"

"""The first block of code defines several metrics which will be used to gauge 
the network's performance. Included in this is the F1 score, which is a 
comprehensive measure of the network's performance on both true positives and 
false positives.
"""
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
    The F1 metric is the harmonic mean of precision and recall
    """
    p = precision(y_true, y_pred)
    r = recall(y_true, y_pred)
    return 2*((p*r)/(p+r+K.epsilon()))

def fix_colors(image):
  array = np.array(image)  
  if array.ndim<3:
    return np.tile(np.expand_dims(array,axis=2),3)
  else:
    return array


# construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-model", "--model", type=str, default=default_net,
  help="name of pre-trained network to use")
ap.add_argument("-t", "--training", type=str, default=default_train,
  help="keys for training sets")
ap.add_argument("-e", "--epochs", type=int, default=10,
  help="nuber of epochs to train the classifier network on")
ap.add_argument("-n", "--samplenumber", type=int, default=250,
  help="number of images to use for training/verification")
ap.add_argument("-m", "--simulationnumber", type=int, default=30,
  help="number of simulations to use for training the classifier")
ap.add_argument("-s", "--seeded", action='store_const',
  const=True,default=False,
  help="flag for using a np.random.seed(0) to reduce randomness of data")
ap.add_argument("-r", "--resampling", action='store_const',
  const=True,default=False,
  help="flag for enabling resampling of the verification data")
ap.add_argument("-pe", "--ploterrors", action='store_const',
  const=True,default=False,
  help="flag for showing errors made by the final model")
ap.add_argument("-p", "--plotting", action='store_const',
  const=True,default=False,
  help="flag for generating the stats plots, altrernative to pres_plot.f1_plot()")

args = vars(ap.parse_args())

DIRS = {
  "polar": "./training_v1/",
  "iss": "./color/ISS/",
  "image": "./IMAGE/"
}

if args["training"] not in DIRS.keys():
  raise AssertionError("The --training command line argument should "
    "be a key in the `DIRS` dictionary")
im_dir = DIRS[args["training"]]
pos_dir = im_dir + "Aurora/"
neg_dir = im_dir + "NoAurora/"

# Store plots here
plotdir = "./plots/"


if args['seeded']:
  np.random.seed(0)
# get lists from the image directories
pos_l = os.listdir(pos_dir)
neg_l = os.listdir(neg_dir)

# Count our images and report
n_pos = len(pos_l)
n_neg = len(neg_l)
print("[INFO] Detected {} positive images and {} negative images"
     .format(n_pos,n_neg))

# Training Parameters #
# Define the number of images to use for each training/ verification
N_im = args['samplenumber']
# Define ratio of images for training
# for this script the ratio will be 1:1
# These are the total numbers for training images
# The images will be split into half posellite images and half non-pos images
nTrain = min(N_im,n_pos,n_neg)
nTest  = min(N_im,n_pos,n_neg)
# nTest  = min(2*N_im,n_pos,n_neg) if args["resampling"] else N_im

# These are the indices used for training images
ind_pos = np.random.permutation(n_pos)[0:(nTrain+nTest)//2]
ind_neg = np.random.permutation(n_neg)[0:(nTrain+nTest)//2]

# items up to nTrain//2 used for training
train_list = ([pos_dir + f for f in np.array(pos_l)[ind_pos[:nTrain//2]]] +
              [neg_dir + f for f in np.array(neg_l)[ind_neg[:nTrain//2]]])
# remainaing items used from verification
test_list  = ([pos_dir + f for f in np.array(pos_l)[ind_pos[nTrain//2:]]] +
              [neg_dir + f for f in np.array(neg_l)[ind_neg[nTrain//2:]]])

# Plot the example images here
# Where the example images will be the first of the ordered images
# This code can be uncommeneted to view the example images with plt.show()
# It is largely unnecesary

# fig,ax = plt.subplots(1,2,figsize=(12,6))
# plt.subplot(1,2,1)
# plt.grid(False)
# plt.title("Example posellite Image")
# im = Image.open(pos_dir+pos_l[ind_pos[0]])
# plt.imshow(im);

# # Example of negative image
# plt.subplot(1,2,2)
# plt.grid(False)
# plt.title("Example No posellite Image")
# im = Image.open(neg_dir+neg_l[ind_neg[0]])
# plt.imshow(im);

"""
# Pre-Trained Network Setup & Feature Extraction
The next code block downloads the Pre-trained network configuration and weights.  
Luckily the chosen network is included in keras, so this process if fairly straightforward. 
"""
#List of valid ptdnn models to be used
MODELS = {
    "vgg16": tf.keras.applications.VGG16,
    "vgg19": tf.keras.applications.VGG19,
    "inceptionv3": tf.keras.applications.InceptionV3,
    "xception": tf.keras.applications.Xception,
    "resnet50": tf.keras.applications.ResNet50,
    "inceptionresnetv2": tf.keras.applications.InceptionResNetV2,
    "mobilenet": tf.keras.applications.MobileNet,
    "mobilenetv2": tf.keras.applications.MobileNetV2,
    "densenet121": tf.keras.applications.DenseNet121,
    "densenet169": tf.keras.applications.DenseNet169,
    "densenet201": tf.keras.applications.DenseNet201,
    "nasnetlarge": tf.keras.applications.NASNetLarge,
    "nasnetmobile": tf.keras.applications.NASNetMobile,
}

# ensure a valid model name was supplied via command line argument
if args["model"] not in MODELS.keys():
  raise AssertionError("The --model command line argument should "
    "be a key in the `MODELS` dictionary")

if args["model"] in ("mobilenet","mobilenetv2"):
  inshape = (224,224,3)
else:
  inshape = (256,256,3)
# Note that the input_shape chosen here is to help with future datasets, but can be changed
# The feature detector is taken from a pre-trained, deep network, built into keras
ptdnn = MODELS[args['model']](
          weights='imagenet',
          include_top=False,
          input_shape=inshape)

feature_shape = ptdnn.output_shape

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

"""
This model will implement the combined model which detects image features 
and classifies the images in one pass. This will be the model which is actually
used for the aurora identification
"""
model = tf.keras.models.Sequential()
#First get the features from ptdnn
model.add(ptdnn)
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

"""This is the part where we load in the data and organize it into batches

The next block generates batches of training data. The feature array output by the 
feature detector network is of size 6 by 6 by 1536 for each image as an example. 

The subsequent block does the same for validation data.
"""
#define the rescale option
datagen = ImageDataGenerator(rescale=1./255)
#batch size affects performance
batch_size = 32
#number of epochs is taken from command line argument, default is 10
epochs = args['epochs']
# create arrays for inputs (x) and expected outputs (y)
x_train = np.array([ fix_colors(np.array(Image.open(fname).resize(inshape[0:2])) )  
                    for fname in train_list])
y_train = np.concatenate((np.ones(nTrain//2), np.zeros(nTrain//2)))
# this is an iterator, trust me.
train_generator = datagen.flow(x_train, y_train, batch_size=batch_size)
#pre-allocate these arrays which are filled in in the loop
train_featrs = np.zeros(
  shape=np.concatenate(([nTrain], feature_shape[1:]))
  )
train_labels = np.zeros(shape=(nTrain))
# look over the batches and compute features and labels note that tese are shuffled
# compared to the x and y arrays
i = 0
for inputs_batch, labels_batch in train_generator:
    print(f"[INFO] Computing Features and Labels for training batch {i}/{nTrain//batch_size}",end='\r')
    # This is the line that finds the features using the ptdnn
    features_batch = ptdnn.predict(inputs_batch)
    #Case for most batches
    if ((i+1) * batch_size < nTrain):
      train_featrs[i * batch_size : (i + 1) * batch_size] = features_batch
      train_labels[i * batch_size : (i + 1) * batch_size] = labels_batch
    #Case for last batch
    else:
      train_featrs[i * batch_size : ] = features_batch[0:nTrain-i*batch_size]
      train_labels[i * batch_size : ] = labels_batch[0:nTrain-i*batch_size]
    i += 1
    #break when done
    if i * batch_size >= nTrain:
        break
# this print is just a new line
print('')
# x and y arrays for verification
x_test = np.array([np.array( fix_colors(Image.open(fname).resize(inshape[0:2])) ) 
                   for fname in test_list])
y_test = np.concatenate((np.ones(nTest//2), np.zeros(nTest//2)))
#iterator
test_generator = datagen.flow(x_test, y_test, batch_size=batch_size)
#pre-allocate
test_featrs = np.zeros(
  shape=np.concatenate(([nTest], feature_shape[1:]))
  )
test_inputs = np.zeros(shape=np.concatenate(([nTest], inshape)) )
test_labels = np.zeros(shape=(nTest))
#iterate
i = 0
for inputs_batch, labels_batch in test_generator:
    print(f"[INFO] Computing Features and Labels for test batch {i}/{nTest//batch_size}",end='\r')
    featrs_batch = ptdnn.predict(inputs_batch)
    if((i+1)*batch_size < nTest):
      test_featrs[i * batch_size : (i + 1) * batch_size] = featrs_batch
      test_inputs[i * batch_size : (i + 1) * batch_size] = inputs_batch
      test_labels[i * batch_size : (i + 1) * batch_size] = labels_batch
    else:
      test_featrs[i * batch_size : ] = featrs_batch[0:nTest-i*batch_size]
      test_inputs[i * batch_size : ] = inputs_batch[0:nTest-i*batch_size]
      test_labels[i * batch_size : ] = labels_batch[0:nTest-i*batch_size]
    i += 1
    if i * batch_size >= nTest:
        break
print('')
print('[INFO] All Features computed successfully')
#hooray!

"""
This is the part where we are going to train and retrain the network m times.
Hope you have time...
"""
# We want to know the shape of our weights
# weight_shape = classifier.get_weights().shape()
# We need a function to reset the weights
def randomize_weights(model,weights=None):
    """Randomly permute the weights in `model`, or the given `weights`.

    This is a fast approximation of re-initializing the weights of a model.

    Assumes weights are distributed independently of the dimensions of the weight tensors
      (i.e., the weights have the same distribution along each dimension).

    :param Model model: Modify the weights of the given model.
    :param list(ndarray) weights: The model's weights will be replaced by a random permutation of these weights.
      If `None`, permute the model's current weights.
    """
    if weights is None:
        weights = model.get_weights()
    # new weights are random
    model.set_weights(
      [np.random.uniform(low=-0.05,high=0.05,size=w.shape) for w in weights]
      )
# end def

results = {}
# Do the training M times
M = args["simulationnumber"]
# Add randomness to the data set we're using by resampling
if(args["resampling"]):
  resamples = np.random.randint( 0, nTest, size=(M,N_im) )
else:
  resamples = np.tile(np.arange(0,nTest),(M,1))
for i in range(M):
  # Reset the weights
  randomize_weights(classifier)
  # print statement
  print(f'[INFO] Training Model Simulation {i+1}/{M}',end='\r')
  # Train
  history = classifier.fit(train_featrs,
    train_labels,
    epochs=epochs,
    batch_size=batch_size,
    validation_data=(
      test_featrs[resamples[i]],
      test_labels[resamples[i]]),
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
"""
Characterizing the proportion: This is done by comparing the simulated F1 scores at each Epoch
to the requirement defined by `req`. The proportion is the ratio of the number of simulations
which succeed at meeting the requirement and the total simulations. A confidence interval is 
defined by the t statistic and the standard deviation of the estimator statistic and the number
of simulations. For the proportion, the standard deviation of the estimator is p*(1-p).

The estimator of mean F1 score at each epoch is also used. the standard deviation for this estimator
is calculated from np.std on each epoch.
"""
if args["plotting"]:
  req = 0.95
  prop = (
    np.sum(np.array(results['val_f1'])>=req,axis=0)
    /M)
  mean_f1 = np.mean(results['val_f1'],axis=0)
  # Confidence level
  level = 0.95
  # Amount of uncertainty based on confidence level
  std_f1 = np.std(results['val_f1'],axis=0)
  unc_f1 = stats.t.ppf(1/2+level/2,df=M-1) * std_f1/np.sqrt(M)
  unc_prop = stats.t.ppf(1/2+level/2,df=M-1) * prop*(1-prop)/np.sqrt(M)

  # Plot for proportion of models meeting requirement
  fig = plt.figure(figsize=(12,8))
  plt.plot(np.arange(epochs),prop,'.-',
    linewidth=2,
    label='Measured Proportion')
  plt.plot(np.arange(epochs),prop+unc_prop,'-',
    color='xkcd:green',
    linewidth=2,
    label=f'{100*level:.0f}% Confidence Interval Upper Limit')
  plt.plot(np.arange(epochs),prop-unc_prop,'-',
    color='xkcd:red',
    linewidth=2,
    label=f'{100*level:.0f}% Confidence Inteval Lower Limit')
  plt.xlim([0,epochs-1])
  plt.ylim([0,1])
  plt.grid(True)
  plt.xlabel('Training Epochs',
    fontsize=16)
  plt.ylabel(f'Proportion Meeting Requirement f1>={req}',
    fontsize=16)
  plt.title(
    f'Propotion of models which meet f1>={req} at Each Epoch\n' \
    f'Feature Detector:{args["model"]}, ' \
    f'Trained on {N_im} Images{(", Resampled" if args["resampling"] else "")}',
    fontsize=20)
  plt.legend(loc='best',fontsize=12)
  # Save this figure to a png even if it isn't a unique name
  savefile = f'{plotdir}proportion_{args["model"]}_e{epochs}_n{N_im}_m{M}.png'
  i=1
  while os.path.isfile(savefile):
    savefile = f'{plotdir}proportion_{args["model"]}_e{epochs}_n{N_im}_m{M}_{i}.png'
    i+=1
  print(f'[INFO] Proportion chart saved to {savefile}')
  plt.savefig(savefile)

  # Plot for mean F1 Score of simulated models
  fig = plt.figure(figsize=(12,8))
  meas = plt.plot(np.transpose(results['val_f1']),'o',
    markersize=8,
    alpha=0.15,
    color='gray')
  plt.plot(np.arange(epochs),mean_f1,'.-',
    linewidth=2,
    label='Mean F1 Score')
  plt.plot(np.arange(epochs),mean_f1+unc_f1,'-',
    color='xkcd:green',
    linewidth=2,
    label=f'{100*level:.0f}% Confidence Interval Upper Limit')
  plt.plot(np.arange(epochs),mean_f1-unc_f1,'-',
    color='xkcd:red',
    linewidth=2,
    label=f'{100*level:.0f}% Confidence Interval Lower Limit')
  plt.axhline(y=req,
    linestyle='--',
    color='xkcd:purple',
    linewidth=3,
    label='F1 Score Requirement')
  plt.xlim([0,epochs-1])
  # plt.ylim([0,1])
  plt.grid(True)
  plt.xlabel('Training Epochs',
    fontsize=16)
  plt.ylabel('Mean F1 Score',
    fontsize=16)
  plt.title(
    f'Mean F1 Scores at Each Epoch\n' \
    f'Feature Detector:{args["model"]}, ' \
    f'Trained on {N_im} Images{(", Resampled" if args["resampling"] else "")}',
    fontsize=20)
  h,l = plt.gca().get_legend_handles_labels()
  h1 = [meas[0]]+h
  l1 = ['F1 Score Measuremets']+l
  plt.legend(h1,l1,fontsize=12,loc='best')
  # Save this figure to a png also
  savefile = f'{plotdir}mean_f1_{args["model"]}_e{epochs}_n{N_im}_m{M}.png'
  i=1
  while os.path.isfile(savefile):
    savefile = f'{plotdir}mean_f1_{args["model"]}_e{epochs}_n{N_im}_m{M}_{i}.png'
    i+=1

  plt.savefig(savefile)
  print(f'[INFO] Mean F1 chart saved to {savefile}')
  plt.show()


if(args['ploterrors']):
  """This next section goes over errors in the validation data and displays a bunch of example images."""

  ground_truth = test_labels
  predictions = model.predict(test_inputs).reshape((nTest,))
  #find where the errors in the testing data are
  errors = np.where((predictions>0.5) != ground_truth)[0]
  print("Number of Errors = {}/{}".format(len(errors),nTest))
  # Create this list thing for plotting examples
  from itertools import chain
  images = chain(errors,range(10))

  for i in images:
    # Print the classification and prediction for this case
    print(f'Image does {"" if ground_truth[i] else "not"} contain aurora,'
          f' Probability: {predictions[i]}')
    # Show the image for reference
    plt.imshow(test_inputs[i])
    plt.title(f'Predicted Aurora Probability = {100*predictions[i]:.2f}%')
    plt.grid(False)
    plt.show()