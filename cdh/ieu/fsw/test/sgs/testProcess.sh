#!/bin/bash

#This spawns a test processes which writes a counter every three seconds to STDOUT when initialized
#Use this to test QtProcess spawning and reading from STDOUT

i="0"
counter=1
while [ $i -lt 1 ]
do
echo $counter
echo $counter >> testProcessOutput
counter=$[$counter+1]
sleep 3
done
