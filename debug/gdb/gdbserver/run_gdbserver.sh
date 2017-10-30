#!/bin/bash

ip=`adb shell ifconfig | grep "Bcast" | cut -d ":" -f 2 | cut -d " " -f 1`
port=2345
gdbserver=/data/gdbserver64
target_bin=crash_no_afl

#echo help info
echo "bxtp device ip is "$ip
echo "### gdb client commad: ###"
echo "target remote $ip:$port ./$target_bin"

# build on server
from=/home/nma1x-2/building-gcc-for-android/toolchain/build/../gcc/gcc-4.9/
# move source files to local path
to=/home/nma1x/bxtp/afl/
echo "set substitute-path $from $to"

echo
#run android gdbserver64 on android device
adb shell $gdbserver $ip:$port /data/$target_bin
