#!/usr/bin/env python
# coding=utf-8

import sys

print '--- 1 ---'
print dir(sys) # get list of attributes for sys module

print '--- 2 ---'
print dir() # get list of attributes for current module

print '--- 3 ---'
a = 5 # create a new variable 'a'
print dir()

print '--- 4 ---'
del a # delete/remove a name
dir()

# note: 使用内建的dir函数来列出模块定义的标识符。标识符有函数、类和变量。
# 当你为dir()提供一个模块名的时候，它返回模块定义的名称列表。如果不提供参数，
# 它返回当前模块中定义的名称列表。
