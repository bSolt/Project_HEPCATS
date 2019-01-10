#!/bin/bash

counter="1"
while [ $counter -eq 1 ]
do
./sim_ieu_send_tlm_pkt
sleep 3
done

