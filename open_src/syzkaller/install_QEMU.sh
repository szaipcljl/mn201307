#!/bin/bash

echo "### enter install_QEMU.sh ###"
#
# QEMU
#
KERNEL=$HOME/linux
IMAGE=$HOME/image

# Install QEMU:
sudo apt-get install kvm qemu-kvm

# Make sure the kernel boots and sshd starts:
# the command in qemu_kernel-boots_sshd-starts.sh

echo "### end of install_QEMU.sh ###"
