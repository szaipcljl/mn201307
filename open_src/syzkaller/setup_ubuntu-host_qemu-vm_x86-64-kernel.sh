#!/bin/bash
# Setup: Ubuntu host, QEMU vm, x86-64 kernel

syzkaller_dir_path=$(pwd)

#
# 1.GCC
#
source ./build_gcc.sh

#
# 2.kernel
#
source ./build_kernel.sh

#
# 3.Image
#
source ./create_debian-wheezy_linux-image.sh

#
# 4.QEMU
#
source ./install_QEMU.sh

#
# 5.Go and 6.syzkaller
#
source ./install_syzkaller.sh
# manager config: my.cfg.

# 7.Run syzkaller manager:
./bin/syz-manager -config=my.cfg

# Now syzkaller should be running, you can check manager status with your web browser at 127.0.0.1:56741.
