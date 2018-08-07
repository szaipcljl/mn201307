#!/usr/bin/env python
# coding=utf-8

# 如何引发异常

# Filename: raising.py
class ShortInputException(Exception):
    '''A user-defined exception class.'''
    def __init__(self, length, atleast):
        Exception.__init__(self)
        self.length = length
        self.atleast = atleast

try:
    s = raw_input('Enter something --> ')
    if len(s) < 3:
        raise ShortInputException(len(s), 3)
        # Other work can continue as usual here
except EOFError:
    print '\nWhy did you do an EOF on me?'
except ShortInputException, x:
    print 'ShortInputException: The input was of length %d, \
was expecting at least %d' % (x.length, x.atleast)
else:
    print 'No exception was raised.'

# note:
# 使用raise语句 引发 异常。你还得指明错误/异常的名称和伴随异常 触发的 异常对象。你
# 可以引发的错误或异常应该分别是一个Error或Exception类的直接或间接导出类。
#
# 我们创建了我们自己的异常类型，其实我们可以使用任何预定义的异常/错误。这个新
# 的异常类型是ShortInputException类。它有两个域——length是给定输入的长度，atleast则是程序
# 期望的最小长度。
# 在except从句中，我们提供了错误类和用来表示错误/异常对象的变量。这与函数调用中的形参
# 和实参概念类似。在这个特别的except从句中，我们使用异常对象的length和atleast域来为用户
# 打印一个恰当的消息。
