#!/bin/bash

for i in {1..127}
do
  	./gc_prompt cmd hepcats haltmdqscan
  	sleep 0.25
  	echo ${i}
done
