#!/bin/sh

export PATH="/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/bin:/usr/unittest/"

show_help()
{
	echo "command:"
	echo "  switch_channel chan0"
	echo "  switch_channel chan1"
	echo "  switch_channel dual"
}

if [ -z "$1" ]; then
	show_help
	exit 0
else
	if [ $1 == "chan0" ]; then
		echo "====== switch to $1 ======"
		keyword="_chan0"
	elif [ $1 == "chan1" ]; then
		echo "====== switch to $1 ======"
		keyword=_chan1
	elif [ $1 == "dual" ]; then
		echo "====== switch to dual ar0144 ======"
		keyword=
	else
		echo "cmd error: $1 !"
		show_help
		exit 1
	fi
fi

CFG=/usr/local/bin/scripts/multi_chan_vin0_1_dual_720p_linear_lcd"${keyword}".lua
echo "CFG: ${CFG}"

test_encode_generic --multi-chan-cfg ${CFG} --lcd 720p  --mixer 1 --osd-mixer b -A --smaxsize 720p -h 720p -b 0 -e -B --smaxsize 720p -h 720p -b 1 -e
#test_encode_generic --multi-chan-cfg ${CFG} --lcd 720p
