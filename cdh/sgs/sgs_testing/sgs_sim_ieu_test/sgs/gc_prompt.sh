#!/bin/bash

##############################################################################
#
# SGS and Simulated IEU Communication Test
#
# Ground Control Prompt
#
# This shell script serves as a substitute for the Ground Control's prompt.
# A prompt input is defined and passed into the gc_prompt.c script.
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
# Created: November 6, 2018
# 
###############################################################################

# Clean up executable:
make clean

# Compile:
make gc_prompt

# Define Ground Control prompt input:
prompt_input_str="cmd hepcats"
#prompt_input_str="cmd hepcats with rsp"
#prompt_input_str="cmd hepcats hold 2018/310-14:15:21"
#prompt_input_str="cmd hepcats with rsp hold 2018/310-14:15:21"

# "Type" it into the Ground Control prompt:
./gc_prompt ${prompt_input_str}