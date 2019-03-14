% Cropping function
%
% This function reads in a photo of the Earth and crops the image so they
% are consistent when input to the classifier
%
% Input: raw image from camera
% Output: ?? x ?? pixel image with Earth at center
%
% Author: Alex Baughman
% Date Created: 1/13/19
% Last Modified: 1/14/19

close all;clear all;clc

%%

% read in image
I = imread('test_image.jpg');
%I = imresize(I,4);

% locate Earth in image
%centers = imfindcircles(I,[500 1000]);

% crop image to ?? x ?? pixels centered about the Earth
centerx = size(I,2)/2;
centery = size(I,1)/2;
square = 1250;
xmin = centerx - square/2;
ymin = centery - square/2;
I_crop = imcrop(I,[xmin,ymin,square,square]); % [xmin,ymin,width,height]

% output image
figure()
imshow(I)
figure()
imshow(I_crop)



