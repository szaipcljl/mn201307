#!/bin/sh
echo 19 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio19/direction
echo 1 > /sys/class/gpio/gpio19/value

#
# I2C Control:
#
# *1.I2C Device ID
#
# *2.
# //SER ID
# //SER Alias
# DES ID
# DES Alias
#
# *3.
# Slave ID
# Slave Alias
#
# 4.
# SCL High Time
# SCL Low Time
#
# *5.
# *I2C Pass Through (bit)
#  I2C Pass Through All (bit)

#step1: access 925
#ds90ub925 address 7'b:0x0C, 8'b: 0x18
echo -n "access to ds90ub925, i2c device id: "
i2cget -y 0 0x0c 0x0

# step2: enable communication with all the remote devices
# I2C Pass Through (bit) 0x03[3]
i2cset -y 0 0x0c 0x03 0xda

# step3: print DES ID
#926 i2c addr 7'bit: 0x2c, 8'b: 0x58
# DES ID (0x06)
echo -n "DES ID: "
i2cget -y 0 0x0c 0x06
#i2cset -y 0 0x0c 0x06 0x58

echo -n "access to 926: "
i2cget -y 0 0x2c 0x0

# step4: set Slave ID(0x07), Slave Alias(0x08)
# slave:IT66121FN, 7'b:0x4c, 8'b: 0x98
i2cset -y 0 0x0c 0x07 0x98
i2cset -y 0 0x0c 0x08 0x98

# reset IT66121FN
# 926.GPO_REG6(0x20) -> IT66121FN.SYSRSTN: 0->1
i2cset -y 0 0x2c 0x20 0x10
#sleep 0.001
usleep 100000
i2cset -y 0 0x2c 0x20 0x90
echo -n "access to IT66121FN, 0x0: "
i2cget -y 0 0x4c 0x0


###############################
# check 925 reg
echo -n "General Status (0x0c): "
i2cget -y 0 0x0c 0x0c


