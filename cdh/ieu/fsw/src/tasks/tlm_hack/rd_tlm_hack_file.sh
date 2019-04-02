#!/bin/bash
#Reads the workaround telemetry file
#This is executed from a ssh tunnel on the SGS
i="0"
while [ $i -eq 0 ]
do
tail /tmp/tlmhackfile
sleep .9
done
