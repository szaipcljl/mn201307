#!/bin/bash

# 忽略这三个信号，防止脚本执行时使用ctrl-C 就退出脚本
trap ":" INT QUIT TSTP

set i=0
set j=0
for((i=0;i<10;))
do
	let "j=j+1"
	echo "-------------j is $j -------------------"
done

