#!/usr/bin/env python
# coding=utf-8

# Filename: lambda.py

def make_repeater(n):
    return lambda s: s*n

twice = make_repeater(2)

print twice('word')
print twice(5)

# note:
# 我们使用了make_repeater函数在运行时创建新的函数对象，并且返回它。lambda语句用
# 来创建函数对象。本质上，lambda需要一个参数，后面仅跟单个表达式作为函数体，而表达式
# 的值被这个新建的函数返回。注意，即便是print语句也不能用在lambda形式中，只能使用表达
# 式。
