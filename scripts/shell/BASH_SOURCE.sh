#!/bin/bash

# source this-script

# If the script is sourced by another script
if [ -n "$BASH_SOURCE" -a "$BASH_SOURCE" != "$0" ]; then
	echo "do_something"
else # Otherwise, run directly in the shell
	echo "do_other"
fi

# 1.取得当前执行的shell文件所在的完整路径 
# 2.执行dirname，并进入父级目录 
# 3.打印当前工作目录的过程
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../" && pwd )"
# DIR=$( cd "$( dirname ${BASH_SOURCE[0]} )/../" && pwd )
# DIR=`cd $( dirname ${BASH_SOURCE[0]} )/../ && pwd`
echo $DIR
echo "ok"

# 获取脚本名称
echo ${BASH_SOURCE-$0}  
echo ${BASH_SOURCE[0]}

