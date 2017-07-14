#!/bin/bash
# Setup: Ubuntu host, QEMU vm, x86-64 kernel

syzkaller_dir_path=$(pwd)

#
# 1.GCC and kernel
#
source $syzkaller_dir_path/build_gcc.sh
source $syzkaller_dir_path/build_kernel.sh

#
# 2.Image
#
source $syzkaller_dir_path/create_debian-wheezy_linux-image.sh

#
# 3.QEMU
#
source $syzkaller_dir_path/install_QEMU.sh

#
# 4.Go and syzkaller
#
# manager config: qemu.cfg.
source $syzkaller_dir_path/install_GO.sh
source $syzkaller_dir_path/install_syzkaller.sh

# 5.Run syzkaller manager:
#2017/07/14 16:27:27 failed to create instance: qemu stopped:
#Could not access KVM kernel module: Permission denied
sudo chmod 666 /dev/kvm
./bin/syz-manager -config=$syzkaller_dir_path/qemu.cfg

# Create config with "type": "adb" and specify adb devices to use
# ./bin/syz-manager -config=adb.cfg

# Now syzkaller should be running, you can check manager status with
# your web browser at 127.0.0.1:56741.
