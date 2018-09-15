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
# (1) Prepare the make file and AmbaConfig file
make sync_build_mkcfg

# (2)     (for single channel)
make s5l22_aibox_config

# (3) Display the menuconfig (select options for multi-channels)
make menuconfig
# choose: 
# Ambarella Board Configuration  ---> ard Revision (AIBOX (S5L66 8Gbit DDR3 @ 912MHz))  ---> (X) AIBOX (S5L66 8Gbit DDR3 @ 912MHz)

# 3. build the firmware
make

#
# After the building process completes, the image can be found in:
#
ls ../../out/<board_type>/images/bst_bld_kernel_lnx_release.bin


see S5L-SDK-009-1.5_Ambarella_S5L_UG_Flexible_Linux_SDK_Code_Building_and_Debug_Environment for more info.
