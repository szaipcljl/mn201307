#!/bin/bash

#(1) 使用objdump把编译生成的bin文件反汇编，helloworld.o，
#把反汇编信息保存到err.txt文件中：
objdump helloworld.o -D > err.txt

#(2) 出错的地址是0xd，使用addr2line定位代码行：
addr2line -C -f -e helloworld.o d
#addr2line -Cfe helloworld.o d
