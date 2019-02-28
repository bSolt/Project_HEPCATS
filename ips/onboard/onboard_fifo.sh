#!/bin/bash

IMG="../RAW/IMG_6591.CR2"
PIPE="./pipe.fifo"

# Make fifo pipe if necessary
if [ ! -f $PIPE ]; then
	echo "Making $PIPE"
	mkfifo $PIPE
fi

#call some scripts
echo "Running C Script"
./write_pipe $IMG $PIPE &

echo "Running Python Script"
# options to python script are [pipe name] [image format] [debug]
# call ips script with the test pipe, test format, and debug flag
python3 ips_ieu_script.py $PIPE test 1


rm $PIPE
