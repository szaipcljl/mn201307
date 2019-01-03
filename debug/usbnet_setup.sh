#!/bin/bash
ip_addr=192.168.2.8

usbnet_name=$(ifconfig | grep enp | cut -d : -f 1 | grep "u")
echo "#mn-usbnet_name:" ${usbnet_name}
sudo ifconfig ${usbnet_name} ${ip_addr}
ifconfig | grep ${ip_addr}
