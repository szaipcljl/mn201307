#!/bin/bash

# core file setting:
ulimit -c
ulimit -c unlimited
ulimit -c

#echo commands
echo "help commands: r bt l q"
echo

#
gcc -g core_dump_test.c

# check "not stripped" key words
file a.out

echo
gdb a.out core


