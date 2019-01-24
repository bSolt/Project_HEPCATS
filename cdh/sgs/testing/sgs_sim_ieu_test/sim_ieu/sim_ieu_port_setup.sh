#!/bin/bash
#Written by: Christopher Peercy for ASEN-4018
# University of Colorado-Aerospace Engineering 
# Senior Projects: Project HEPCATS
# Configures virtual serial ports for testing
socat -d -d pty,raw,echo=0 pty,raw,echo=0

