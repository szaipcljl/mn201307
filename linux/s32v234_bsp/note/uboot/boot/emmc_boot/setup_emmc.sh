#!/bin/sh

#1. flash uboot to emmc
dd if=u-boot.s32 of=/dev/mmcblk0 seek=8 bs=512

#2. create partition and format emmc
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
" | fdisk /dev/mmcblk0

mkfs.vfat -n boot /dev/mmcblk0p1
#mke2fs -t ext3 -L rootfs /dev/mmcblk0p2
mkfs.ext3  -L rootfs /dev/mmclk0p2

#3. create mount point and mount
mkdir /run/media
mkdir /run/media/mmcblk0p1
mkdir /run/media/mmcblk0p2

mount /dev/mmcblk0p1 /run/media/mmcblk0p1
mount /dev/mmcblk0p2 /run/media/mmcblk0p2

#4. copy image...
cp Image rda600.dtb  /run/media/mmcblk0p1/
cp -raf fakeroot/* /run/media/mmcblk0p2/

