export PATH=$PATH:$(pwd)/../prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin
export BUILD_DIR=./out
export make CROSS_COMPILE=aarch64-linux-android-
make distclean
#make sptsharklfpga_config
make sp9838aea_5mod_config
make -j4
