#!/bin/bash

# build firmware

#
# 1. Configure the Toolchain 
#
source build/env/aarch64-linaro-gcc.env


#
# 2. configure the image under boards/<board_type>.
#
cd boards/aibox/
# (1)
make sync_build_mkcfg

# (2)     (for single channel)
make s5l22_aibox_config

# (3)  (select options for multi-channels)
make menuconfig
# choose: 
# Ambarella Board Configuration  ---> ard Revision (AIBOX (S5L66 8Gbit DDR3 @ 912MHz))  ---> (X) AIBOX (S5L66 8Gbit DDR3 @ 912MHz)

# 3. build the firmware
make

#
# After the building process completes, the image can be found in:
#
ls ../../out/<board_type>/images/bst_bld_kernel_lnx_release.bin
