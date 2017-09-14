build commands
==============

build bxt o:
------------
repo init -u ssh://android.intel.com/manifests -b android/master -m r0 \
&& repo sync -c \
&& ./device/intel/mixins/mixin-update \
&& source build/envsetup.sh \
&& lunch gordon_peak-userdebug \
&& make flashfiles -j12 2>&1 | tee build.log

build kernel:
-------------
make bootimage -j$(nproc) 2>&1 | tee build.log
>> 2>&1 can output error to build.log

dump the commands being used for compilation
--------------------------------------------
(time make -j24 showcommands) 2>&1 | tee buildcmds

To then see the tools in use:
----------------------------
grep ^prebuilts buildcmds | cut -d " " -f 1 | sort | uniq
