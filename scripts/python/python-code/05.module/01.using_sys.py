#!/usr/bin/env python
# coding=utf-8

# Filename: using_sys.py
# ./01.using_sys.py we are arguments

# 标准库模块
import sys

print 'The command line arguments are:'

for i in sys.argv:
    print i

print '\n\nThe PYTHONPATH is', sys.path, '\n'

print sys.argv[0]

# note: 利用import语句输入 sys模块。sys模块包含了与Python解释器和它的环境有关的函数。
# 当Python执行import sys语句的时候，它在sys.path变量中所列目录中寻找sys.py模块。如果找到
# 了这个文件，这个模块的主块中的语句将被运行，然后这个模块将能够被你 使用 。注意，初
# 始化过程仅在我们 第一次 输入模块的时候进行。

# sys.path包含输入模块的目录名列表.
