#!/bin/sh

sudo adb root
sudo adb disable-verity

sudo adb remount
sudo adb push $OUT/system/lib64/hw/sensors.gmin.so system/lib64/hw/sensors.gmin.so
sudo adb push $OUT/system/lib/hw/sensors.gmin.so system/lib/hw/sensors.gmin.so
sudo adb push sensor_hal_config_default.xml system/etc/


