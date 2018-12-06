#!/bin/bash
breakout=0
while [ $breakout -eq 0 ]
do
	read -p "PROMPT:" myinput
	echo "$myinput"
	./$myinput
done

