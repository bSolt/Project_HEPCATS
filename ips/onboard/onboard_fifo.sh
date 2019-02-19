#!/bin/bash

IMG="../RAW/IMG_6591.CR2"
PIPE="./pipe.fifo"

if [ ! -f $PIPE ]; then
	echo "Making $PIPE"
	mkfifo $PIPE
fi

#call some scripts
echo "Running C Script"
./write_pipe $IMG $PIPE &

echo "Running Python Script"
python3 ips_ieu_script.py $PIPE


rm $PIPE