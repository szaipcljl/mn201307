#!/usr/bin/env python
# coding=utf-8

# Filename: func_global.py
def func():
    global x
    print 'x is', x
    x = 2
    print 'Changed local x to', x

x = 50
func()
print 'Value of x is', x

# note: global 声明x是全局的, global x, y, z
