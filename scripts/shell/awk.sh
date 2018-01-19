#!/bin/bash
date | awk '{print $5}'
date | awk -F ':' '{print $2}'
awk -F '%' '{print $2}' date.sh

echo "------"
echo "a b c  d" |awk '{print}'
echo "a b c  d" |awk '{print $0}'
echo "a b c d" |awk '{print $1}'
echo "a b c d" |awk '{print $2}'
echo "a b c d" |awk '{print $3}'
echo "a b c d" |awk '{print $4}'
echo "a b c  d" |awk '{print $1, $2, $3, $4}'

echo "----"
echo "a b c  d" |awk '{$1=$1; print}'
