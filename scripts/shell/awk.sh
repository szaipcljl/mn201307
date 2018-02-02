#!/bin/bash
date | awk '{print $5}'
date | awk -F ':' '{print $2}'
awk -F '%' '{print $2}' date.sh

echo "1-----"
echo "a b c  d" |awk '{print}'
echo "a b c  d" |awk '{print $0}'
echo "a b c d" |awk '{print $1}'
echo "a b c d" |awk '{print $2}'
echo "a b c d" |awk '{print $3}'
echo "a b c d" |awk '{print $4}'
echo "a b c  d" |awk '{print $1, $2, $3, $4}'

echo "2----"
echo "a b c  d" |awk '{$1=$1; print}'

echo "3----"
# 多个空格替换成1个，并删除开头的空格
echo "     a" | sed 's/  */ /g' | sed 's/^ *//g'
echo "     a" | sed 's/  */ /g' | cut -d ' ' -f1
echo "     a" | awk '{print $1}'
echo "--"
echo "     a" | sed 's/  */ /g' | sed 's/^ *//g' | cut -d ' ' -f1
