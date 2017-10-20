#!/bin/bash
if [ -z "$1" ] ; then
	keywords=
else
	keywords=$1-
fi

suffix=$(date +%F-%T)

if [ ! -d "./log_tmp" ]; then
	echo "create directory: ./log_tmp"
	mkdir ./log_tmp
fi

logdir=./log_tmp
prefix=$(find "$logdir" -type f | wc -l)
logpath=$logdir/$prefix-$keywords$suffix.log

#sudo python build.py --toolchain x86_64 2>&1 | tee $logpath
touch $logpath
echo -n "end time: "
date +%F-%T
echo "log path: $logpath"


