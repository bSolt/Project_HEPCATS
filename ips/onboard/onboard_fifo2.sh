#!/bin/bash

IMG="../RAW/Aurora1.RAW"
PIPE="./pipe.fifo"

if [ ! -f $PIPE ]; then
	echo "Making $PIPE"
	mkfifo $PIPE
fi

#call some scripts
echo "Running C Script"
./write_pipe $IMG $PIPE &

echo "Running Python Script"
python3 ips_ieu_script.py $PIPE ieu2 1


rm $PIPE