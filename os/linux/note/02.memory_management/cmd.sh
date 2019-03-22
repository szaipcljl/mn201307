#!/bin/bash

gcc mem_area.c

# size - list section sizes and total size.
echo "利用size命令看程序的各段大小(程序编译的静态统计):"
size a.out

