#!/usr/bin/env python
# coding=utf-8

def powersum(power, *args):
    '''Return the sum of each argument raised to specified power.'''
    total = 0
    for i in args:
        total += pow(i, power)
    return total

print powersum(2, 3, 4)
print powersum(2, 10)


# note:
# 在函数中接收元组和列表
# 当要使函数接收元组或字典形式的参数的时候，有一种特殊的方法，它分别使用*和**前缀。
# 这种方法在函数需要获取可变数量的参数的时候特别有用。
#
# 由于在args变量前有*前缀，所有多余的函数参数都会作为一个元组存储在args中。如果使用的
# 是**前缀，多余的参数则会被认为是一个字典的键/值对。
