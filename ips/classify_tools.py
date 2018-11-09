"""Tools for loading keras models and classifying images"""

import tensorflow as tf
import numpy as np
from PIL import Image
from tensorflow.keras import backend as K
import cv2
import keras #for the old model

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

def load_keras_model(model="mobilenet"):
    model = MODELS[model]
    model = model(weights="imagenet",includeTop=true)

def load_raw(filename):
    import rawpy
    raw = rawpy.imread(filename)
    rgb = raw.postprocess()
    return rgb

def resize_withPIL(rgb_image,new_size):
    im = Image.fromarray(rgb_image)
    im = im.resize(new_size)
    return im

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

def classify_with_prototype(image,model):
    image = np.array(image)/255
    image = np.expand_dims(image,axis=0)
    return model.predict(image)[0][0]

def show_image_overlay(image,text):
    image = np.array(image)
    image = cv2.cvtColor(image,cv2.COLOR_RGB2BGR)
    cv2.putText(image, text,
        (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)
    cv2.imshow("Classification", image)
    cv2.waitKey(0)


def classify_downloaded_files():
    prototype = "/home/braden/project_hepcats/ips/transfer/model_nasnetm_10.h5"

    rawfiles = ["/home/braden/Downloads/IMG_6556.CR2",
             "/home/braden/Downloads/IMG_6561.CR2",
             "/home/braden/Downloads/IMG_6566.CR2"]

    files = ["/home/braden/Downloads/P-51_Mustang.jpg",
            "/home/braden/Downloads/f9.jpg",
            "/home/braden/Downloads/Boeing.png",
            "/home/braden/Downloads/still_testing.jpg"]

    classifier = tf.keras.models.load_model(prototype,
        custom_objects={'recall': recall, 
                        'f1' : f1} )
    # classifier.compile(optimizer=tf.keras.optimizers.RMSprop(lr=2e-4),
    #           loss='binary_crossentropy',
    #           metrics=['acc',recall,f1])

    print("[INFO] Classifier network was loaded")

    for file in files:
        im0 = Image.open(file)
        im = im0.resize((256,256))
        # im = load_raw(file)
        # im = resize_withPIL(im,(256,256))
        c=classify_with_prototype(im,classifier)
        # print(c)
        show_image_overlay(im0,"Rocket = {:.2f}%".format(c*100))

def classify_projector_files():
    prototype = "/home/braden/Downloads/prototype.h5"
    rawfiles = ["/home/braden/Downloads/IMG_6556.CR2",
             "/home/braden/Downloads/IMG_6561.CR2",
             "/home/braden/Downloads/IMG_6566.CR2"]

    files = ["/home/braden/Downloads/P-51_Mustang.jpg",
            "/home/braden/Downloads/f9.jpg",
            "/home/braden/Downloads/Boeing.png",
            "/home/braden/Downloads/still_testing.jpg"]

    ptdnn = keras.applications.InceptionResNetV2(
        weights='imagenet',
        include_top=False,
        input_shape=(256,256,3))

    print("[INFO] Feature Detector network was loaded")

    classifier = keras.models.load_model(prototype,
        custom_objects={'recall': recall, 
                        'f1' : f1} )

    print("[INFO] Classifier network was loaded")

    model = keras.models.Sequential()
    model.add(ptdnn)
    model.add(keras.layers.Flatten())
    model.add(classifier)
    # classifier.compile(optimizer=tf.keras.optimizers.RMSprop(lr=2e-4),
    #           loss='binary_crossentropy',
    #           metrics=['acc',recall,f1])

    for file in rawfiles:
        im0 = load_raw(file)
        im = resize_withPIL(im0,(256,256))
        # im = load_raw(file)
        # im = resize_withPIL(im,(256,256))
        c=classify_with_prototype(im,model)
        # print(c)
        show_image_overlay(im0,"Satellite = {:.2f}%".format(c*100))

def main():
    classify_projector_files()
    input('Press Enter to continue...')
    classify_downloaded_files()

if __name__ == '__main__':
    main()
# load classifier from h5 with tf.keras.models.load_model