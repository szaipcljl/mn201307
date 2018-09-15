#!/usr/bin/env python
# coding=utf-8

# Filename:simplestclass.py

class Person:
    pass #An empty block

p = Person()
print p

# note:
# 类的方法与普通的函数只有一个特别的区别——它们必须有一个额外的第一个参数名称，但是
# 在调用这个方法的时候你不为这个参数赋值，Python会提供这个值。这个特别的变量指对象本
# 身，按照惯例它的名称是self。
#
# 你一定很奇怪Python如何给self赋值以及为何你不需要给它赋值。举一个例子会使此变得清
# 晰。假如你有一个类称为MyClass和这个类的一个实例MyObject。当你调用这个对象的方法
# MyObject.method(arg1, arg2)的时候，这会由Python自动转为MyClass.method(MyObject, arg1,
# arg2)——这就是self的原理了

#
# 空白块，它由pass语句表示
