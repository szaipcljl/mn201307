#!/bin/bash
# mv adbPush.sh to the project root dir
adb push out/target/product/cht_vr/system/lib/libsensorcalitool.so /system/lib/
adb push out/target/product/cht_vr/system/lib64/libsensorcalitool.so /system/lib64/
adb push out/target/product/cht_vr/system/bin/sensorcalitool /system/bin/
