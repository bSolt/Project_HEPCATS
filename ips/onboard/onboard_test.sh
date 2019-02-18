#!/bin/bash

IMG="../RAW/IMG_6591.CR2"
PIPE_IN="/dev/pts/3"
PIPE_OUT="/dev/pts/4"

echo "Running C Script"
./write_read $IMG $PIPE_IN $PIPE_OUT &

echo "Running Python Script"
python3 detection.py $PIPE_OUT 