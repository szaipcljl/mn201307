#/bin/bash
#
# adb
#
# 编译完代码后,将adb_fastboot.sh拷贝到编译产生的out/所在目录下执行
sudo cp out/host/linux-x86/bin/adb /usr/bin/
sudo chmod a+x /usr/bin/adb

# 创建spreadtrum usb 专有 VID
[-d ~/.android] || mkdir ~/.android; echo 0x1782 > ~/.android/adb_usb.ini
cat ~/.android/adb_usb.ini

sudo adb kill-server
sudo adb start-server

# 错误解决办法
#echo "SUBSYSTEM==\"usb\", SYSFS{idVendor}==\"1782\", MODE=\"0666\"" >> /etc/udev/rules.d/51-android.rules

##=========== sudo adb shell时，错误解决办法 ======================

#如果失败，错误码error:
#insufficient permissions for device

#sudo adb devices，如果列出设备，失败，错误码为：
#List of devices attached
#????????????    no permissions

#添加列表到：/etc/udev/rules.d/51-android.rules
#echo "SUBSYSTEM==\"usb\", SYSFS{idVendor}==\"1782\", MODE=\"0666\"" >> /etc/udev/rules.d/51-android.rules
#-- 1782 为展讯设备号

#查看所有 usb 设备：
#user@host:~$ lsusb
#Bus 001 Device 026: ID 1782:5d04 Spreadtrum Communications Inc.
#Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
#==================================================================

#fastboot install
sudo cp out/host/linux-x86/bin/fastboot /usr/bin
sudo chmod a+x /usr/bin/fastboot

