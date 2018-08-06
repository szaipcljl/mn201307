#!/usr/bin/env python
# coding=utf-8

# Filename:method.py

class Person:
    def sayHi(self):
        print 'Hello, how are you?'

p = Person()
p.sayHi()

#This short example can also be written as Person().sayHi()
Person().sayHi()

# note:
# sayHi方法没有任何参数，但仍然在函数定义时有self
