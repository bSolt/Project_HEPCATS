#!/bin/bash
#Reads the output of the tlm hack file
i=1
lasttail=0
while [ $i -eq 1 ]
do
check="$(tail /tmp/mdqhackfile)" #tail the file to reduce loop time on oversample check

if [ "$check" != "$lasttail" ] 
#check to see if we have oversampled the data
then
	#if the data is unique 
	cat /tmp/mdqhackfile
	lasttail=$check #set the uniqueness identifier
fi
#to be sure we get all the data, sample at the packet creation rate's nyquist freqency, by sleeping
sleep 0.08695
done

