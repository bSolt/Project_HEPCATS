#!/bin/bash

for i in {1..130}
do
  	./gc_prompt cmd hepcats haltmdqscan
  	sleep 0.5
  	echo ${i}
done