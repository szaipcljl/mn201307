#!/bin/bash
# Setup: Ubuntu host, QEMU vm, x86-64 kernel

syzkaller_dir_path=$(pwd)

#
# 1.GCC and kernel
#
source ./build_gcc.sh
source ./build_kernel.sh

#
# 2.Image
#
source ./create_debian-wheezy_linux-image.sh

#
# 3.QEMU
#
source ./install_QEMU.sh

#
# 4.Go and syzkaller
#
source ./install_GO.sh
source ./install_syzkaller.sh
# manager config: my.cfg.

# 5.Run syzkaller manager:
./bin/syz-manager -config=qemu.cfg

# Create config with "type": "adb" and specify adb devices to use
# ./bin/syz-manager -config=adb.cfg

# Now syzkaller should be running, you can check manager status with
# your web browser at 127.0.0.1:56741.
