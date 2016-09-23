#!/bin/sh

sudo adb remount
sudo adb push $OUT/system/lib64/hw/sensors.r2_cht_ffd.so system/vendor/lib64/hw/sensors.gmin.so
sudo adb push $OUT/system/lib/hw/sensors.r2_cht_ffd.so system/vendor/lib/hw/sensors.gmin.so
sudo adb push sensor_hal_config_default.xml system/etc/


