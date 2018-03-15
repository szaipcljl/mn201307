#!/bin/bash

echo "auto build IDA600"

##########################################
# Download the wuji's tools and install it
##########################################
root_folder=$PWD
echo "$root_folder"

git clone git@121.196.213.107:wuji/tools.git
cd tools/engineering 
./install $root_folder

########################################
# Download the RDA600 & wuji source code
########################################
cd $root_folder

repo init -u git@121.196.213.107:RDA600/manifest.git
repo sync

##########################
# Build the RDA600 project
##########################
source build/env/aarch64-linaro-gcc.env

cd boards/rda600
make sync_build_mkcfg
make rda600_config
make defconfig_public_linux
make menuconfig_public_linux

make -j$(nproc)

