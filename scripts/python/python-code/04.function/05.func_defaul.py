#!/usr/bin/env python
# coding=utf-8

# Filename: func_default.py

def say(message, times = 1):
    print message * times

say('Hello')
say('World', 5)

# note: 只有在形参表末尾的那些参数可以有默认参数值
