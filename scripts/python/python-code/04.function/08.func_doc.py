#!/usr/bin/env python
# coding=utf-8

# Filename: func_doc.py

def printMax(x, y):
    '''Prints the maximum of two numbers.

    The two values must be integers.'''
    x = int(x) # convert to integers, if possible
    y = int(y)

    if x > y:
        print x, 'is maximum'
    else:
        print y, 'is maximum'

printMax(3, 5)
print printMax.__doc__

# note: DocStrings(文档字符串) 帮助你的程序文档更加简单易懂，你应该尽量使用它.
# DocStrings也适用于模块和类

# 文档字符串的惯例是一个多行字符串，它的首行以大写字母开始，句号结尾。第二行是空行，
# 从第三行开始是详细的描述。 强烈建议 你在你的函数中使用文档字符串时遵循这个惯例。

# Python把 每一样东西 都作为对象，包括这个函数。 使用__doc__调用printMax函数的文档字符串属性
