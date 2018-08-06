#!/usr/bin/env python
# coding=utf-8

# Filename: class_init.py
class Person:
    def __init__(self, name):
        self.name = name
    def sayHi(self):
        print 'Hello, my name is', self.name

p = Person('Chris')
p.sayHi()

# This short example can also be written as Person('Swaroop').sayHi()
Person('Chris').sayHi()

#note:
# __init__方法在类的一个对象被建立时，马上运行。
# 这个方法可以用来对你的对象做一些你希望的 初始化 。
#
# __init__方法类似于C++、C#和Java中的 constructor 。
