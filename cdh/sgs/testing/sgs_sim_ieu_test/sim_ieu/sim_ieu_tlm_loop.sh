#!/bin/bash

counter="1"
while [ $counter -eq 1 ]
do
echo "SIM IEU: Sending telemetry packet."
./../../../../testing/sgs_sim_ieu_test/sim_ieu/sim_ieu_send_tlm_pkt
sleep 3
done

