#!/usr/bin/env python
# coding=utf-8

# Filename: cat.py

import sys

def readfile(filename):
    '''Print a file to the standard output.'''
    f = file(filename)
    while True:
        line = f.readline()
        if len(line) == 0:
            break
        print line, # notice comma
    f.close()

# Script starts from here
if len(sys.argv) < 2:
    print 'No action specified.'
    sys.exit()

if sys.argv[1].startswith('--'):
    option = sys.argv[1][2:]
    # fetch sys.argv[1] but without the first two characters
    if option == 'version':
        print 'Version 1.2'
    elif option == 'help':
        print '''\
This program prints files to the standard output.
Any number of files can be specified.
Options include:
--version : Prints the version number
--help : Display this help'''
    else:
        print 'Unknown option.'
        sys.exit()
else:
    for filename in sys.argv[1:]:
        readfile(filename)

# note:
# 在Python程序运行的时候，即不是在交互模式下，在sys.argv列表中总是至少有一个项目。它就
# 是当前运行的程序名称，作为sys.argv[0]（由于Python从0开始计数）。其他的命令行参数在这
# 个项目之后。
# help(sys.exit)
# 名称cat是 concatenate 的缩写，它基本上表明了程序的功能——它可以在输出打
# 印一个文件或者把两个或两个以上文件连接/级连在一起打印
