#!/bin/sh

##############
# common
##############
export PATH="/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/bin:/usr/unitest/"
:<<!
VIN_FPS=20
CVBS_RES=576

echo "----1. modprobe frame buffer"
modprobe ambarella_fb resolution=720x${CVBS_RES} mode=clut8bpp

echo "----2. load dsp microcode"
load_ucode /lib/firmware
!

###############
# multi chan
###############
# ar0144 -> B6FS -> B6NS-> S5L
# ar0230 -> B6FS -> B6NS-> S5L
# Vout: cvbs

init.sh --na;

modprobe b6 id=0x101
modprobe ar0144_mipi_brg brg_id=0x0
modprobe ar0230_brg brg_id=0x1

test_tuning -a &

#test_encode_generic --multi-chan-cfg /usr/local/bin/scripts/multi_chan_vin0_1_720_1080p_linear.lua
test_encode_generic --multi-chan-cfg ./multi_chan_vin0_1_720_1080p_linear.lua --cvbs 576i
test_encode_generic -A --smaxsize 720p -h 720p -b 0 -e -B --smaxsize 1080p -h 1080p -b 1 -e

###############
# single
###############
# ar0144 -> B6FS -> B6NS-> S5L
# Vout: cvbs
:<<!
modprobe b6 id=0x101
modprobe ar0144_mipi_brg brg_id=0x0
modprobe ar0230_brg brg_id=0x1

init.sh -na
test_tuning -a &
test_encode -i 0 --cvbs ${CVBS_RES}i -X --bmaxsize 720p --bsize 720p --binsize 720p
test_encode -A -h 720p -e
!
