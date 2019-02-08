#!/bin/bash

############
#
#
# Written by Christopher Peercy
# For ASEN-4018
# Used by C&T GUI as dummy telemetry stream 
#
# DEVLOPEMENT TESTING USE ONLY

#############

i="0"
counter=1

## Setup a loop to run until program is terminated
while [ "$i" -lt 1 ]
do

#Generate random numbers to make sure telemetery updates

IEU_Temp=$(($RANDOM % 91))
IEU_Volt=$(($RANDOM % 6))
IMG_Rate="0.2"

#Simulate not having an image on every downlink packet
HAS_IMAGE=$(($RANDOM % 2))

MAG_rate=$(($RANDOM % 100))
HAS_MAG=$(($RANDOM % 2))

#Simulate receiving or not reciveing a mag packet by checking if has mag is true
if [ "$HAS_MAG" -eq 1 ]
then 
# If the simulated packet has a mag packet, make a random number to increment counter
MAG_packets=$(($RANDOM % 200))
else
MAG_packets=0
fi

#Show some command counts (will not match with sent on GUI

cc_exe=$(($RANDOM % 100))
cc_ack=$(($RANDOM % 100))
cc_err=$(($RANDOM % 100))


#Put everything out to STDOUT

echo $IEU_Temp,$IEU_Volt,$IMG_Rate,$HAS_IMAGE,$MAG_rate,$MAG_packets,$cc_exe,$cc_ack,$cc_err

sleep 2
done




