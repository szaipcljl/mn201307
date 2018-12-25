#!/bin/bash
# ds90ub914 power down
echo 85 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio85/direction
echo 1 > /sys/class/gpio/gpio85/value

# camera module(ar0144) power on enable pin
echo 16 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio16/direction
echo 1 > /sys/class/gpio/gpio16/value
