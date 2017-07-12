#!/bin/bash

#
# 2.kernel
#
# KCOV was committed upstream in Linux kernel version 4.6 and can be enabled by
# configuring the kernel with CONFIG_KCOV=y.

GCC=$HOME/gcc-7.1.0
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
sed -i 's/# CONFIG_KCOV is not set/CONFIG_KCOV=y/' $KERNEL/.config
sed -i 's/# CONFIG_DEBUG_INFO is not set/CONFIG_DEBUG_INFO=y/' $KERNEL/.config
sed -i 's/# CONFIG_KASAN is not set/CONFIG_KASAN=y/' $KERNEL/.config

# Since enabling these options results in more sub options being available, we
# need to regenerate config. Run this and press enter each time when prompted for
# some config value to leave it as default:

make oldconfig
# note!!!:
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
make CC=$GCC/install/bin/gcc -j$(nproc)

# Now you should have vmlinux (kernel binary) and bzImage (packed kernel image):
ls $KERNEL/vmlinux
ls $KERNEL/arch/x86/boot/bzImage
