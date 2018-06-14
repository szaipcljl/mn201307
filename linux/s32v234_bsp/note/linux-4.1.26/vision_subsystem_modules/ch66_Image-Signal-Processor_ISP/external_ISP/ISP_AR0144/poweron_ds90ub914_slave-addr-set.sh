#!/bin/bash

# ds90ub914 power down
echo 85 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio85/direction
echo 1 > /sys/class/gpio/gpio85/value

# camera module(xc7027+ar0144) power on enable pin
echo 16 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio16/direction
echo 1 > /sys/class/gpio/gpio16/value

#
# config 914 regs
#
# 913 slave addr: 7'bit: 0x5d
i2cset -y 1 0x61 0x7 0xBA
# xc7027 slave addr: 7'bit: 0x1b
i2cset -y 1 0x61 0x8 0x36
i2cset -y 1 0x61 0x10 0x36
# ar0144 slave addr: 7'bit: 0x10
i2cset -y 1 0x61 0x9 0x20
i2cset -y 1 0x61 0x11 0x20

# i2cset -y I2C_BUSS 7'bit_i2c_addr reg val

#
# check ds90ub914 regs
#
i2cget -y 1 0x61 0x7
i2cget -y 1 0x61 0x8
i2cget -y 1 0x61 0x10
i2cget -y 1 0x61 0x9
i2cget -y 1 0x61 0x11
