#!/bin/bash

OUT_DIR=../../out/rda600
UBOOT_OUT=${OUT_DIR}/bootloader/u-boot
KERNEL_OUT=${OUT_DIR}/kernel/linux-4.9_rda600-release

MKIMAGE=${UBOOT_OUT}/tools/mkimage
#IMAGE=${KERNEL_OUT}/arch/arm64/boot/Image
IMAGE=${KERNEL_OUT}/arch/arm64/boot/Image.gz
UIMAGE=${KERNEL_OUT}/arch/arm64/boot/uImage

LOADADDR=0x8007FFC0
ENTRY_POINT=0x8007FFC0

#${MKIMAGE} -A arm64 -O linux -T kernel -C none -a 0x80080000 -e 0x80080000 -n "Linux" -d ${IMAGE} ${UIMAGE}
${MKIMAGE} -A arm64 -O linux -T kernel -C gzip -a 0x80080000 -e 0x80080000 -n "Linux" -d ${IMAGE} ${UIMAGE}
#${MKIMAGE} -A arm64 -O linux -T kernel -C gzip -a ${LOADADDR} -e ${ENTRY_POINT} -n "Linux" -d ${IMAGE} ${UIMAGE}

echo "copy to sd card"
cp ../../out/rda600/kernel/linux-4.9_rda600-release/arch/arm64/boot/uImage /media/mlinux/boot/
sync
