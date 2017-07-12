#!/bin/bash

#
# QEMU
#
KERNEL=$HOME/linux
IMAGE=$HOME/image
# Install QEMU:
sudo apt-get install kvm qemu-kvm

# Make sure the kernel boots and sshd starts:
qemu-system-x86_64 \
	-kernel $KERNEL/arch/x86/boot/bzImage \
	-append "console=ttyS0 root=/dev/sda debug earlyprintk=serial slub_debug=QUZ"\
	-hda $IMAGE/wheezy.img \
	-net user,hostfwd=tcp::10021-:22 -net nic \
	-enable-kvm \
	-nographic \
	-m 2G \
	-smp 2 \
	-pidfile vm.pid \
	2>&1 | tee vm.log

# syzkaller login: root

# After that you should be able to ssh to QEMU instance in another terminal:
ssh -i $IMAGE/ssh/id_rsa -p 10021 -o "StrictHostKeyChecking no" root@localhost

# To kill the running QEMU instance:
#kill $(cat vm.pid)
