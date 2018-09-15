#!/bin/bash

OUT_DIR=../../../out/rda600
UBOOT_OUT=${OUT_DIR}/bootloader/u-boot
KERNEL_OUT=${OUT_DIR}/kernel/linux-4.9_rda600-release

MKIMAGE=${UBOOT_OUT}/tools/mkimage

############################################
# create itb: new-style (FDT) kernel booting
############################################
${MKIMAGE} -f kernel_fdt.its kernel_fdt.itb
#${MKIMAGE} -l kernel_fdt.itb
