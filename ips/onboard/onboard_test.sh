#!/bin/bash

#Test image (RAW FORMAT)
IMG="../RAW/IMG_6591.CR2"
# Virtual comm ports, get them from this command
# socat -d -d pty,raw,echo=0 pty,raw,echo=0
PIPE_IN="/dev/pts/2"
PIPE_OUT="/dev/pts/3"

echo "Running C Script"
./write_read $IMG $PIPE_IN $PIPE_OUT &

echo "Running Python Script"
python3 detection.py $PIPE_IN $PIPE_OUT 