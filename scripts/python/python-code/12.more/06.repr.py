#!/usr/bin/env python
# coding=utf-8

i = []
i.append('item')

print `i`

print repr(i)


# note:
# repr函数用来取得对象的规范字符串表示。反引号（也称转换符）可以完成相同的功能。注
# 意，在大多数时候有eval(repr(object)) == object。
#
# epr函数和反引号用来获取对象的可打印的表示形式。你可以通过定义类的__repr__
# 方法来控制你的对象在被repr函数调用的时候返回的内容。
