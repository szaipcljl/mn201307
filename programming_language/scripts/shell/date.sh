#!/bin/bash

# %u   day of week (1..7); 1 is Monday
# %V   ISO week number, with Monday as first day of week (01..53)
# ww29.6
date +%F\(%V.%u\)-%T
