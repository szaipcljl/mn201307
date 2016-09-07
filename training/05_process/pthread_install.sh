#!/bin/bash


sudo apt-get install glibc-doc -y --force-yes
# man -k pthread或apropos pthread可以查找到当前manpages中关于pthread的手册。

# 安装manpages-posix-dev
sudo apt-get install manpages-posix manpages-posix-dev -y --force-yes
