#!/usr/bin/env python
# coding=utf-8

# Filename: using_tuple.py

zoo = ('wolf', 'elephant', 'penguin')
print 'Number of animals in the zoo is', len(zoo)

new_zoo = ('monkey', 'dolphin', zoo)
print 'Number of animals in the new zoo is', len(new_zoo)
print 'All animals in new zoo are', new_zoo
print 'Animals brought from old zoo are', new_zoo[2]
print 'Last animal brought from old zoo is', new_zoo[2][2]

# note: 元组和列表十分类似，只不过元组和字符串一样是 不可变的 即你不能修改元组。
# 元组通过圆括号中用逗号分割
# 一个空的元组由一对空的圆括号组成，如myempty = ()
# 想要的是一个包含项目2的元组的时候，你应该指明singleton = (2 , )
