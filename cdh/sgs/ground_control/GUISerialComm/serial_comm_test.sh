#!/bin/bash

##############################################################################
#
# Serial Communications Test
#
# -------------------------------------------------------------------------- #
#
# Input Arguments:
# - N/A
#
# Output Arguments:
# - N/A
# 
# -------------------------------------------------------------------------- #
#
# Benjamin Spencer
# ASEN 4018
# Project HEPCATS
# Subsystem: C&DH
# Created: October 28, 2018
# 
###############################################################################

# Compile project:b
make read > /dev/null
make write

# Start read script in a new terminal:
xterm -fa 'Monospace' -fs 10 -e ./read_main &
xterm_1_pid=$! # Save PID for later

# Sleeps one second:
sleep 1

# Start write script:
xterm -fa 'Monospace' -fs 10 -e ./write_main &
xterm_2_pid=$! # Save PID for later

# Pause:
read -n 1 -s -r -p "Press any key to stop test " 
printf "\n"

# Kill xterms:
#kill ${xterm_1_pid}
#kill ${xterm_2_pid}
