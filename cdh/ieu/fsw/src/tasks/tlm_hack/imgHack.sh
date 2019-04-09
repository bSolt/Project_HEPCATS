#!/bin/bash

##We need to check to see if there's a new image in the temp folder
#diff the image to see what's changed. If nothings changed diff will return nothing
newImg=$(diff /tmp/img.raw /home/xenomai/img.old)
if [ -n "$newImg" ]
then
echo 1
supress=$(cp /tmp/img.raw  /home/xenomai/img.old)
else
echo 0
fi
diff /home/xenomai/img.old /tmp/img.raw
