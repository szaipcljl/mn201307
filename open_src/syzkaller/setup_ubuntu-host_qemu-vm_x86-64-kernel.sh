#!/bin/bash
# Setup: Ubuntu host, QEMU vm, x86-64 kernel

#
# 1.GCC
#
source ./build_gcc.sh

#
# 2.kernel
#
KERNEL=$HOME/linux
mkdir $KERNEL

# Checkout Linux kernel source:
git clone https://github.com/torvalds/linux.git $KERNEL

# Generate default configs:
cd $KERNEL
make defconfig
make kvmconfig

# Now we need to enable some config options required for syzkaller. Edit .config
# file manually and enable:
#CONFIG_KCOV=y
#CONFIG_DEBUG_INFO=y
#CONFIG_KASAN=y
#CONFIG_KASAN_INLINE=y

# Since enabling these options results in more sub options being available, we
# need to regenerate config. Run this and press enter each time when prompted for
# some config value to leave it as default:

make oldconfig
# 'CONFIG_KASAN_INLINE=y' => follow bellow after make oldconfig
#*
#* kmemcheck: trap use of uninitialized memory
#*
#kmemcheck: trap use of uninitialized memory (KMEMCHECK) [N/y/?] n
#KASan: runtime memory debugger (KASAN) [Y/n/?] y
  #Instrumentation type
    #> 1. Outline instrumentation (KASAN_OUTLINE) (NEW)
		#2. Inline instrumentation (KASAN_INLINE) (NEW)
		  #choice[1-2]: 2

# Build the kernel with previously built GCC:
make CC='$GCC/install/bin/gcc' -j($nproc)

# Now you should have vmlinux (kernel binary) and bzImage (packed kernel image):
ls $KERNEL/vmlinux
ls $KERNEL/arch/x86/boot/bzImage 

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
