#!/bin/bash
i=1
ssh -p 22 xenomai@10.42.0.218 './project_hepcats/cdh/ieu/fsw/src/tasks/tlm_hack/imgHackInit.sh'
while [ $i -eq 1 ]
do
newImg=$(ssh -p 22 xenomai@10.42.0.218 './project_hepcats/cdh/ieu/fsw/src/tasks/tlm_hack/imgHack.sh') 
if [ $newImg -eq 1 ]
then
scp -P 22 xenomai@10.42.0.218:/tmp/img.raw .
fi
sleep 30
done
