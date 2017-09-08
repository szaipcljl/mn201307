make bootimage -j$(nproc)
=========================

make: Entering directory '/home/nma1x/disk2/nma1x-wk/bxt-o/kernel/bxt'
make[1]: Entering directory '/home/nma1x/disk2/nma1x-wk/bxt-o/out/target/product/gordon_peak/obj/kernel'
  GEN     ./Makefile
scripts/kconfig/conf  --olddefconfig Kconfig
#
# configuration written to .config
#

FAILED: /home/nma1x/disk2/nma1x-wk/bxt-o/out/target/product/gordon_peak/obj/kernel/arch/x86/boot/bzImage 
/bin/bash -c "(make -j6 SHELL=/bin/bash -C kernel/bxt O=/home/nma1x/disk2/nma1x-wk/bxt-o/out/target/product/gordon_peak/obj/kernel ARCH=x86_64 INSTALL_MOD_PATH=. CROSS_COMPILE=\"/home/nma1x/disk2/nma1x-wk/bxt-o/prebuilts/misc/linux-x86/ccache/ccache /home/nma1x/disk2/nma1x-wk/bxt-o/out/host/linux-x86/poky/sysroots/x86_64-pokysdk-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-\" CCACHE_SLOPPINESS=include_file_mtime,file_macro ) && (make -j6 SHELL=/bin/bash -C kernel/bxt O=/home/nma1x/disk2/nma1x-wk/bxt-o/out/target/product/gordon_peak/obj/kernel ARCH=x86_64 INSTALL_MOD_PATH=. CROSS_COMPILE=\"/home/nma1x/disk2/nma1x-wk/bxt-o/prebuilts/misc/linux-x86/ccache/ccache /home/nma1x/disk2/nma1x-wk/bxt-o/out/host/linux-x86/poky/sysroots/x86_64-pokysdk-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-\" CCACHE_SLOPPINESS=include_file_mtime,file_macro modules ) && (make -j6 SHELL=/bin/bash -C kernel/bxt O=/home/nma1x/disk2/nma1x-wk/bxt-o/out/target/product/gordon_peak/obj/kernel ARCH=x86_64 INSTALL_MOD_PATH=. CROSS_COMPILE=\"/home/nma1x/disk2/nma1x-wk/bxt-o/prebuilts/misc/linux-x86/ccache/ccache /home/nma1x/disk2/nma1x-wk/bxt-o/out/host/linux-x86/poky/sysroots/x86_64-pokysdk-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-\" CCACHE_SLOPPINESS=include_file_mtime,file_macro INSTALL_MOD_STRIP=1 modules_install )"

make olddefconfig
------------------------------------------------------------
scripts/kconfig/conf  --olddefconfig Kconfig

update defconfig after running make ARCH=x86_64 olddefconfig
