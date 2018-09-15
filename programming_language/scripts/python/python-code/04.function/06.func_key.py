#!/usr/bin/env python
# coding=utf-8

# Filename: func_key.py

def func(a, b=5, c=10):
    print 'a is', a, 'and b is', b, 'and c is', c

func(3, 7)
func(25, c=24)
func(c=50, a=100)

# note: 通过命名来为这些想要的参数赋值.(使用关键参数来指定参数值)
