build kernel:
=============
cd kernel/bxt
cp arch/x86/configs/x86_64_defconfig .config
make arch=x86_64 menuconfig
cp .config arch/x86/configs/x86_64_defconfig
make mrproper  


add some error on Makefile
--------------------------
[ 68% 20/29] build /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.check
FAILED: /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.check /bin/bash -c \
"([[ -e /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config ]] && mv -f /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.save ; true ) && \
(cat kernel/config-lts/v4.9/bxt/android/x86_64_defconfig > /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config) && \
(make -j6 SHELL=/bin/bash -C kernel/bxt O=/home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel ARCH=x86_64 INSTALL_MOD_PATH=.  CROSS_COMPILE=\"/home/nma1x/disk2/nma1x-wk/bxtp-o/prebuilts/misc/linux-x86/ccache/ccache /home/nma1x/disk2/nma1x-wk/bxtp-o/out/host/linux-x86/poky/sysroots/x86_64-pokysdk-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-\" CCACHE_SLOPPINESS=include_file_mtime,file_macro olddefconfig ) && \
(kernel/bxt/scripts/diffconfig /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.old /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config > /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.check) && \
([[ -e /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.save ]] && mv -f /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.save /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config ; true ) && \
(if [[ -s /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.check ]] ; then        echo \"CHECK KERNEL DEFCONFIG FATAL ERROR :\" ; echo \"Kernel config copied from kernel/config-lts/v4.9/bxt/android/x86_64_defconfig has some config issue.\" ;        echo \"Final '.config' and '.config.old' differ.  This should never happen.\" ;        echo \"Observed diffs are :\" ;          cat /home/nma1x/disk2/nma1x-wk/bxtp-o/out/target/product/gordon_peak/obj/kernel/.config.check ;       echo \"Root cause is probably that a dependancy declared in Kconfig is not respected\" ;         echo \"or config was added in Kconfig but value not explicitly added to defconfig.\" ; echo \"Recommanded method to generate defconfig is menuconfig tool instead of manual edit.\" ;         exit 1;  fi )" 
