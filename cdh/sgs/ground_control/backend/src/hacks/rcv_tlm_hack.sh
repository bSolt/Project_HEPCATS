#!/bin/bash
## Establishes a ssh connection with the ieu and reads the hk telemetry file
##Written by: Christopher Peercy 
# University of Colorado Aerospace Engineering Senior Projects
# Project HEPCATS

#For this to work, you MUST add the SGS public key to the IEU authorized key file.
#On the sgs:
# user~$ ssh-keygen
# scp -P 22 .ssh/id_rsa.pub xenomai@localhost:
# ssh to xenomai then:
# cat id_rsa.pub > .ssh/authorized_keys
# ensure pub key authentication is turned on in /etc/ssh/sshd_config and authorized keys file is correct.

ssh -p 22 xenomai@10.42.0.218 ./project_hepcats/cdh/ieu/fsw/src/tasks/tlm_hack/rd_tlm_hack_file.sh

