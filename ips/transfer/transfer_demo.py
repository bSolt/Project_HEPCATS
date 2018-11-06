"""Transfer demo script is built to build a prototype classifier network 
which uses transfer learning to distinguish between two types of images 
downloaded from imagenet"""

import os
import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from PIL import Image
import tensorflow as tf
import argparse

K = tf.keras.backend
default_net = "inceptionresnetv2"

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

# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
# ap.add_argument("-i", "--image", required=True,
#   help="path to the input image")
ap.add_argument("-model", "--model", type=str, default=default_net,
  help="name of pre-trained network to use")
args = vars(ap.parse_args())

# this is the rockets sysnet
pos_id = "n04265904" 
# this is the airplanes sysnet
neg_id = ["n02691156"]

# store the images here
pos_dir = "./rockets/"
neg_dir = "./planes/"

# do the downloading
if not os.path.exists(pos_dir):
  print(os.popen("imagenetscraper " + pos_id +' '+ pos_dir).read())
else:
  print("[INFO] Using existing positive images")
if not os.path.exists(neg_dir):  
  print(os.popen("imagenetscraper " + neg_id +' '+ neg_dir).read())
else:
  print("[INFO] Using existing negative images")

"""the following code block chooses the training and validation data. 
For the purposes of demonstration, 600 images are used for training and 
300 are used for validation. Examples for each class are also shown by 
this block. The randomization can be changed by changing `np.random.seed()`.
The important variables in this code are `train_list` and `test_list`. 
These lists contain the relative paths to all of the training images selected. 
It should be noted that these lists are sorted with the first half being the 
positive classification cases for both."""

# get lists from the image directories
pos_l = os.listdir(pos_dir)
neg_l = os.listdir(neg_dir)

# Count our images and report
n_pos = len(pos_l)
n_neg = len(neg_l)
print("[INFO] Detected {} positive picutres and {} negative pictures"
     .format(n_pos,n_neg))

# Training Parameters #
# Define the number of images to use for each classification
N_im = 600
# Define ratio of images for training
rTrain = 0.75
rTest = 1-rTrain
# These are the total numbers for training images
# The images will be split into half posellite images and half non-pos images
nTrain = 2*round(N_im*rTrain)
nTest  = 2*round(N_im*rTest)

# Randomizing
np.random.seed(1)

# These are the indices used for training images
ind_pos = np.random.permutation(n_pos)[0:N_im]
ind_neg = np.random.permutation(n_neg)[0:N_im]

# items up to nTrain//2 used for training
train_list = ([pos_dir + f for f in np.array(pos_l)[ind_pos[:nTrain//2]]] +
              [neg_dir + f for f in np.array(neg_l)[ind_neg[:nTrain//2]]])
# remainaing items used from verification
test_list  = ([pos_dir + f for f in np.array(pos_l)[ind_pos[nTrain//2:]]] +
              [neg_dir + f for f in np.array(neg_l)[ind_neg[nTrain//2:]]])


# Plot the example images here
# Where the example images will be the first of the ordered images
# This code can be uncommeneted to view the example images with show()

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

"""# Pre-Trained Network Setup & Feature Extraction

The next code block downloads the Pre-trained network configuration and weights.  
Luckily the chosen network is included in keras, so this process if fairly straightforward. 
The code also displays a summary of the network's layers.
"""

MODELS = {
    "vgg16": tf.keras.applications.VGG16,
    "vgg19": tf.keras.applications.VGG19,
    "inceptionv3": tf.keras.applications.InceptionV3,
    "xception": tf.keras.applications.Xception,
    "resnet50": tf.keras.applications.ResNet50,
    "inceptionresnetv2": tf.keras.applications.InceptionResNetV2,
    "mobilenet": tf.keras.applications.MobileNet,
    "densenet121": tf.keras.applications.DenseNet121,
    "densenet169": tf.keras.applications.DenseNet169,
    "densenet201": tf.keras.applications.DenseNet201,
    "nasnetlarge": tf.keras.applications.NASNetLarge,
    "nasnetmobile": tf.keras.applications.NASNetMobile,
}

# esnure a valid model name was supplied via command line argument
if args["model"] not in MODELS.keys():
  raise AssertionError("The --model command line argument should "
    "be a key in the `MODELS` dictionary")

# Note that the input_shape chosen here is to help with future datasets
# The feature detector is taken from a pre-trained, deep network, built into keras
ptdnn = MODELS[args['model']](
          weights='imagenet',
          include_top=False,
          input_shape=(256, 256, 3))

feature_shape = ptdnn.output_shape

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

model = tf.keras.models.Sequential()
model.add(ptdnn)
model.add(classifier)
model.layers[0].trainable=False
# Display layer information for reference
model.summary()

# classifier options including metrics and loss function
ptdnn.compile()
classifier.compile(optimizer=tf.keras.optimizers.RMSprop(lr=2e-4),
              loss='binary_crossentropy',
              metrics=['acc',recall,f1])
print('[INFO] Models compiled successfully')

# from keras.utils import plot_model
# plot_model(model, to_file='model.png')

"""This is the part where we load in the data and organize it into batches

The next block generates batches of training data. The feature array output by the 
feature detector network is of size 6 by 6 by 1536 for each image. These dimensions 
are flattened in the output because the classifier network will work on 1D data.

The subsequent block does the same for validation data.
"""

from tensorflow.keras.preprocessing.image import ImageDataGenerator

datagen = ImageDataGenerator(rescale=1./255)
batch_size = 32
epochs = 10

x_train = np.array([np.array(Image.open(fname).resize((256,256)).convert("RGB")) 
                    for fname in train_list])
y_train = np.concatenate((np.ones(nTrain//2), np.zeros(nTrain//2)))

# print(x_train.shape)

train_generator = datagen.flow(x_train, y_train, batch_size=batch_size)
 
train_featrs = np.zeros(
  shape=np.concatenate(([nTrain], feature_shape[1:]))
  )
train_labels = np.zeros(shape=(nTrain))

i = 0
for inputs_batch, labels_batch in train_generator:
    print("[INFO] Computing Features and Labels for training batch {}".format(i),end='\r')
    features_batch = ptdnn.predict(inputs_batch)
    if ((i+1) * batch_size < nTrain):
      train_featrs[i * batch_size : (i + 1) * batch_size] = features_batch
      train_labels[i * batch_size : (i + 1) * batch_size] = labels_batch
    else:
      train_featrs[i * batch_size : ] = features_batch[0:nTrain-i*batch_size]
      train_labels[i * batch_size : ] = labels_batch[0:nTrain-i*batch_size]
    i += 1
    if i * batch_size >= nTrain:
        break
        
print('')
# train_featrs = np.reshape(train_featrs, (nTrain, 6 * 6 * 1536))

x_test = np.array([np.array(Image.open(fname).resize((256,256)).convert("RGB")) 
                   for fname in test_list])
y_test = np.concatenate((np.ones(nTest//2), np.zeros(nTest//2)))

# print(x_test[0].shape)

test_generator = datagen.flow(x_test, y_test, batch_size=batch_size)

test_featrs = np.zeros(
  shape=np.concatenate(([nTest], feature_shape[1:]))
  )
test_inputs = np.zeros(shape=(nTest, 256, 256, 3))
test_labels = np.zeros(shape=(nTest))

i = 0
for inputs_batch, labels_batch in test_generator:
    print("[INFO] Computing Features and Labels for test batch {}".format(i),end='\r')
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
        
print('[INFO] All Features computed successfully')
# test_inputs = np.reshape(test_inputs, (nTest, 6 * 6 * 1536))

"""# Building the Classifier Network

Next we must implement our own classifier network to be trained on the data. Luckily, using Keras functions, this processes is fairly straightforward.

The first block of code defines several metrics which will be used to gauge the network's performance. Included in this is the F1 score, which is a comprehensive measure of the network's performance on both true positives and false positives.
"""

# Fit this to our data please!
history = classifier.fit(train_featrs,
                    train_labels,
                    epochs=epochs,
                    batch_size=batch_size,
                    validation_data=(test_featrs,test_labels))

"""The next code block sets up and trains the classifier network. This simple classifier network only have input, hidden layer, dropout, and output layers. The input layer corresponds to the the 5 by 5 by 1536 array generated by the feature detector network, which was flattened earlier. These features are used to determine the values of the hidden layer which has 256 neurons. The following layer to this is a dropout layer. this dropout unit randomly ignores half of the values from the hidden layer. This is useful for preventing overfitting. The output layer is just a single neuron with a sigmoid activation. A sigmoid activation is used here because its output can be interpreted as a probability that the image is a posellite.

Other options specified here are in the last two lines of the code. First, the model was copiled and several options were chosen. The chosen optimizer is known as RMSprop which is known to have very good learning characteristics. The `lr` variabile passed to this stands for learning rate and simply paramaterizes how quickly the network attempts to change weight values by during training. The loss function was specified as a binary, cross-entropy loss becase this is also known to yield good learning values. Metrics that will be recorded are specified as accuracy, recall, and F1 score.

In the last line, training settings defined above are passed to the `fit()` method. Namely a batch size of 20 images and a total epoch count of 50.
"""


"""# Characterizing the Classifications

The first block of code for verifying network performance simply plots the information stored during the training process. There are plots for the metrics tracked in training, accuracy, recall, F1, as well as loss.
"""

plt.plot(history.history['acc'])
plt.plot(history.history['val_acc'])
plt.title('Model accuracy')
plt.ylabel('Accuracy')
plt.xlabel('Epoch')
plt.legend(['Train', 'Test'], loc='upper left')
plt.show()

plt.plot(history.history['recall'])
plt.plot(history.history['val_recall'])
plt.title('Model recall')
plt.ylabel('Recall')
plt.xlabel('Epoch')
plt.legend(['Train', 'Test'], loc='upper left')
plt.show()

plt.plot(history.history['f1'])
plt.plot(history.history['val_f1'])
plt.title('Model F1 score')
plt.ylabel('F1 score')
plt.xlabel('Epoch')
plt.legend(['Train', 'Test'], loc='upper left')
plt.show()

# Plot training & validation loss values
plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.title('Model loss')
plt.ylabel('Loss')
plt.xlabel('Epoch')
plt.legend(['Train', 'Test'], loc='upper left')
plt.show()

input('model machine broken have good day')

# from keras.utils import plot_model
# plot_model(model, to_file='model.png')

"""This next section goes over errors in the validation data and displays a bunch of example images."""

# fnames = test_generator.filenames
 
ground_truth = test_labels
predictions = model.predict(test_features).reshape((300,))
# prob = model.predict(test_features)
 
errors = np.where((predictions>0.5) != ground_truth)[0]
print("No of errors = {}/{}".format(len(errors),nTest))

# Some old debug
# print(predictions.reshape((300,)).shape,ground_truth.shape)
# print(errors)
# # print((predictions>0.5)[0:10],ground_truth[0:10])
# print(predictions[64], ground_truth[64])

from itertools import chain
images = chain(errors,range(50))

for i in images:
#     pred_class = np.argmax(prob[errors[i]])
#     pred_label = idx2label[pred_class]
     
    print('Classification:{}, Prediction: {:.3f}'.format(
        ground_truth[i],
        predictions[i]))
     
    plt.imshow(test_inputs[i])
    plt.grid('off')
    plt.show()

"""The following is inteded to test an integrated classify images function and measure its speed."""

import time

def classify_image(image_data):
  n = image_data.shape[0]
  im_mat = np.asarray(image_data).reshape(n,256,256,3)
  return model.predict(im_mat)

N = 100;

# Make some examples
poss = np.random.randint(0,n_pos,N)
negs = np.random.randint(0,n_neg,N)

ex_list = ([pos_dir + f for f in np.array(pos_l)[poss]] + 
           [neg_dir + f for f in np.array(neg_l)[negs]])

dt=0;

for i in range(2*N):
  im = Image.open(ex_list[i]).resize((256,256)).convert("RGB")
  im = np.expand_dims(np.asarray(im)/255,axis=0)
  t0 = time.time();
  prediction = classify_image(im)
  dt += time.time()-t0;
  print("Original Label:{:i}, Prediction:{:.2f}".format(i<N,prediction[0][0]))
  # plt.imshow(im[0])
  # plt.grid(False)
  # plt.show()
  

print("Average classification time: {:8g}".format(dt/(2*N)))

from google.colab import files

model.save('prototype.h5')

files.download('prototype.h5')

files.download('prototype.h5')
print('Yay, my file downloaded correctly and easily!')

print(os.popen('ls').read())