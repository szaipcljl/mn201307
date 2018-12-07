#!/bin/bash

echo "i2cget -y 1 0x5d 0x5 #mode select"
i2cget -y 1 0x5d 0x5 #mode select

echo "i2cget -y 1 0x5d 0x0c # general statu"
i2cget -y 1 0x5d 0x0c # general statu

echo "i2cget -y 1 0x5d 0x2a # CRC Errors"
i2cget -y 1 0x5d 0x2a # CRC Errors

echo "i2cget -y 1 0x5d 0x2d #inject forward channel errorIAV_IOC_IMG_GET_STATISTICS"
i2cget -y 1 0x5d 0x2d #inject forward channel errorIAV_IOC_IMG_GET_STATISTICS

echo "i2cget -y 1 0x5d 0x3 # general configuration"
i2cget -y 1 0x5d 0x3 # general configuration

echo "i2cget -y 1 0x5d 0x35 #PLL clock overwrite"
i2cget -y 1 0x5d 0x35 #PLL clock overwrite
