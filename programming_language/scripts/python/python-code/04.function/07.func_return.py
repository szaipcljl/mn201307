#!/usr/bin/env python
# coding=utf-8

# Filename: func_return.py

def maximum(x, y):
    if x > y:
        return x
    else:
        return y

print maximum(2, 3)


# note: 除非你提供你自己的return语句，每个函数都在结尾暗含有return None语句。
def someFunction():
    pass # pass语句在Python中表示一个空的语句块。
print someFunction()
