#!/system/bin/sh

./stm32flash -a 0x39 -R -i 360,-341,341:-360,-341,341 -r /data/sh_fw_version -S 0x8019000:0x9 /dev/i2c-3

echo "Sensorhub firmware version:\n"
cat /data/sh_fw_version
echo "\n"
