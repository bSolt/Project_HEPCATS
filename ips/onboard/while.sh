#!/bin/bash

in1=$1
in2=$2

while [ $in1 -lt $in2 ]; do
	echo $in1
	in1=(($in1++))
done
