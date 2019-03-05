#!/bin/bash

for i in {1..256}
do
  	./gc_prompt cmd hepcats magoff
  	sleep 0.25
  	echo ${i}
done
