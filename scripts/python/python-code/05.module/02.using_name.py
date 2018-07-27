#!/usr/bin/env python
# coding=utf-8

# Filename: using_name.py

if __name__ == '__main__':
    print 'This program is being run by itself'
else:
    print 'I am being imported from another module'

# 只在程序本身被使用时运行主块，在它被别的模块输入时不运行主块
