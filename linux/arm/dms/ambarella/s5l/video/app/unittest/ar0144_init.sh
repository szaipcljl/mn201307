#!/bin/sh
# ar0144 -> B6FS -> B6NS-> S5L
# Vout: cvbs

export PATH="/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/bin:/usr/unitest/"

VIN_FPS=20
CVBS_RES=576

echo "----1. modprobe frame buffer"
modprobe ambarella_fb resolution=720x${CVBS_RES} mode=clut8bpp

echo "----2. load dsp microcode"
load_ucode /lib/firmware

init.sh -na
test_tuning -a &
test_encode -i 0 --cvbs ${CVBS_RES}i -X --bmaxsize 720p --bsize 720p --binsize 720p
test_encode -A -h 720p -e

