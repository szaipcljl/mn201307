#!/bin/bash

#1. ulimit –c 查看core dump机制是否使能，若为0则默认不产生core
#dump，可以使用ulimit –c unlimited使能core dump
ulimit -c
ulimit -c unlimited

#2. cat /proc/sys/kernel/core_pattern
#查看core文件默认保存路径，默认情况下是保存在应用程序当前目录下，但是如果应用程序中调用chdir()函数切换了当前工作目录，则会保存在对应的工作目录
echo "core file saved path:"
cat /proc/sys/kernel/core_pattern

#3. echo “/data/xxx/<core_file>” > /proc/sys/kernel/core_pattern
#指定core文件保存路径和文件名，其中core_file可以使用以下通配符：

#%% 单个%字符
#%p 所dump进程的进程ID
#%u 所dump进程的实际用户ID
#%g 所dump进程的实际组ID
#%s 导致本次core dump的信号
#%t core dump的时间 (由1970年1月1日计起的秒数)
#%h 主机名
#%e 程序文件名

#4. ulimit –c [size]
#指定core文件大小，默认是不限制大小的，如果自定义的话，size值必须大于4，单位是block（1block
#= 512bytes）
