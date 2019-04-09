#!/bin/bash
#called to initialize img.old so we dont pass 2.3MB around the IEU constantly.
tail /tmp/img.raw >> /home/xenomai/img.old
