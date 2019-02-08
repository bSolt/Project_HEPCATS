#!/bin/bash

# Author: Braden Solt
# Date Created: 12-27-2018

# Check if enough arguments
if [[ $# <2 ]]; then
	echo "Not enough arguments"
	exit 1
fi

echo "using directory $1 and source $2"
# Create the directory in the local directory
DIR=./$1/

# Make the cdf directory if necesary
if [[ ! -d $DIR ]]; then
	echo "Creating $DIR"
	mkdir $DIR
fi
# extract the CDF files
echo "Extracting $2 to $DIR"
tar -C $DIR -xvf $2
# Call the python script
echo "Collecting Images from CDF"
python3 collect.py -d $DIR
# Remove the original archive and the CDF files
echo "Removing $2"
rm $2
echo "Removing $DIR"
rm -r $DIR