#!/bin/bash

# i2c bus num
export I2C_BUS_NUM=1

#
# ds90ub954 deserializer
#
export primary_devaddr_7bit=0x30
# register
export FPD3_PORT_SEL_ADDR=0x4c
export RX_PORT_CTL_ADDR=0x0c
export BCC_CONFIG=0x58
export SER_ALIAS_ID_ADDR=0x5C
export SLAVE_ID0_ADDR=0x5D
export SLAVE_ALIAS_ID0_ADDR=0x65
export I2C_DEVICE_ID_REG=0x00

# 913 i2c addr
# remote 913 SER slave addr: 7'bit: 0x5d
export REMOTE_SER_ADDR_8BIT=0xBA

# ar0144
# ar0144 slave addr: 7'bit: 0x10
export AR0144_ADDR_8BIT=0x20

#
# ds90ub914 power down
#
echo 8 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio8/direction
echo 1 > /sys/class/gpio/gpio8/value

#
# camera module(xc7027+ar0144) power on enable pin
#
echo 0 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio0/direction
echo 1 > /sys/class/gpio/gpio0/value

#
# config 954 deserializer regs
#
i2cset -y ${I2C_BUS_NUM} ${primary_devaddr_7bit} ${FPD3_PORT_SEL_ADDR}  0x01
i2cset -y ${I2C_BUS_NUM} ${primary_devaddr_7bit} ${RX_PORT_CTL_ADDR}  0x81
i2cset -y 1 0x30 ${BCC_CONFIG} 0x58
sleep 1
i2cset -y ${I2C_BUS_NUM} ${primary_devaddr_7bit} ${SER_ALIAS_ID_ADDR} ${REMOTE_SER_ADDR_8bit}

# ar0144
i2cset -y ${I2C_BUS_NUM} ${primary_devaddr_7bit} ${SLAVE_ID0_ADDR} ${AR0144_ADDR_8BIT}
i2cset -y ${I2C_BUS_NUM} ${primary_devaddr_7bit} ${SLAVE_ALIAS_ID0_ADDR} ${AR0144_ADDR_8BIT}

# i2cset -y I2C_BUSS 7'bit_i2c_addr reg val

#
# check ds90ub954 deserializer regs
#
i2cget -y ${I2C_BUS_NUM} ${primary_devaddr_7bit} ${SER_ALIAS_ID_ADDR}
i2cget -y ${I2C_BUS_NUM} ${primary_devaddr_7bit} ${SLAVE_ID0_ADDR}
i2cget -y ${I2C_BUS_NUM} ${primary_devaddr_7bit} ${SLAVE_ALIAS_ID0_ADDR}
