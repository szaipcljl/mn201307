#!/bin/bash

echo "---(1)---选择匹配行的第三列"

awk '/46 bits/         {print $0}' /proc/cpuinfo
awk '/46 bits/         {print $3}' /proc/cpuinfo
awk '/46 bits/{print $6}' /proc/cpuinfo

echo "----"
awk '/CPU architecture/{print $3}' /proc/cpuinfo


echo "---(2)---选择匹配行的第三列"
# 筛选出匹配的行
sed -n '/46 bits/p' /proc/cpuinfo

#TAB替换为空格
sed -n '/46 bits/p' /proc/cpuinfo | sed 's/\t/ /g' | cut -d ' ' -f3
echo "---(3)---"
#多个空格替换成一个空格
echo "a  b   c d     f" | sed 's/  */ /g' | cut  -d ' ' -f3
i=3
echo "a  b   c d     f" | sed 's/  */ /g' | cut  -d ' ' -f$i

