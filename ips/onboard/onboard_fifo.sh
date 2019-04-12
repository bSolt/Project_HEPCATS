#!/bin/bash

IMG_DIR="$HOME/OITLraw/"
PIPE="./pipe.fifo"

if [ $# -gt 0 ]; then
	MODEL=$1
else
	MODEL=../models/retrained_1.h5
fi

echo "Using $MODEL as the model file"

# Make fifo pipe if necessary
if [ ! -f $PIPE ]; then
	echo "Making $PIPE"
	mkfifo $PIPE
fi

echo "Running Python Script"
# options to python script are [pipe name] [image format] [debug]
# call ips script with the test pipe, test format, and debug flag
python3 ips_ieu_script.py -p $PIPE -t ieu -m $MODEL -k -v &

#call some scripts
for FILE in $(ls $IMG_DIR); do
	echo "Running C Script for image $FILE"
	PATH=$IMG_DIR$FILE
	./write_pipe $PATH $PIPE
	# The next ready message
	echo -n k >> $PIPE
done

rm $PIPE
echo "Script exited"
