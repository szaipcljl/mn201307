#!/bin/sh

PROJECT_OUT=out/rda600
UBOOT_OUT=${PROJECT_OUT}/bootloader/u-boot/
KERNEL_OUT=${PROJECT_OUT}/kernel/linux-4.1_rda600
RELEASE_KERNEL_OUT=${PROJECT_OUT}/kernel/linux-4.1_rda600-release

IMAGE=${KERNEL_OUT}/arch/arm64/boot/Image
DTB=${KERNEL_OUT}/arch/arm64/boot/dts/freescale/rda600.dtb
CSE=boards/rda600/config/uboot/cse.bin
ROOTFS=${PROJECT_OUT}/fakeroot

#USER_NAME=chris
USER_NAME=$USER
SD_DEV=/dev/sdb
SD_BOOT_MOUNT_DIR=/media/${USER_NAME}/boot
SD_ROOTFS_MOUNT_DIR=/media/${USER_NAME}/rootfs


#1. create partition
echo "====== 1 of 5: create partitions ======"

echo "d

d

n
p
1
2048
+255M
n
p
2


t
1
c
t
2
83
w
" | sudo fdisk ${SD_DEV}

sleep 1

#2. format the partitions
echo "====== 2 of 5: format the partitions ======"
umount ${SD_DEV}1 ${SD_DEV}2
echo "y" | sudo mkfs.vfat -n boot ${SD_DEV}1

echo "y" | sudo mkfs.ext3 -L rootfs ${SD_DEV}2

sleep 5

#3. flash uboot
echo "====== 3 of 5: flash uboot ======"
mkdir ${SD_BOOT_MOUNT_DIR} ${SD_ROOTFS_MOUNT_DIR}
mount ${SD_DEV}1 ${SD_BOOT_MOUNT_DIR}
mount ${SD_DEV}2 ${SD_ROOTFS_MOUNT_DIR}

dd if=${UBOOT_OUT}/u-boot.s32 of=${SD_DEV} seek=8 bs=512

#4. copy image...
echo "====== 4 of 5: copy Image/rda600.dtb/cse.bin... ======"
sudo cp ${IMAGE} ${DTB} ${CSE}  ${SD_BOOT_MOUNT_DIR}
echo "====== 5 of 5: copy rootfs... ======"
sudo cp -raf ${ROOTFS}/* ${SD_ROOTFS_MOUNT_DIR}

echo "sync..."
sync
