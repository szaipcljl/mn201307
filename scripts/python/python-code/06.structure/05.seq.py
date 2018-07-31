#!/usr/bin/env python
# coding=utf-8

# Filename: seq.py

shoplist = ['apple', 'mango', 'carrot', 'banana']
# Indexing or 'Subscription' operation
print 'Item 0 is', shoplist[0]
print 'Item 1 is', shoplist[1]
print 'Item 2 is', shoplist[2]
print 'Item 3 is', shoplist[3]
print 'Item -1 is', shoplist[-1]
print 'Item -2 is', shoplist[-2]

# Slicing on a list
print 'Item 1 to 3 is', shoplist[1:3]
print 'Item 2 to end is', shoplist[2:]
print 'Item 1 to -1 is', shoplist[1:-1]
print 'Item start to end is', shoplist[:]

# Slicing on a string
name = 'swaroop'
print 'characters 1 to 3 is', name[1:3]
print 'characters 2 to end is', name[2:]
print 'characters 1 to -1 is', name[1:-1]
print 'characters start to end is', name[:]

# note:
# 列表、元组和字符串都是序列
# 序列的两个主要特点是索引操作符和切片操作符。
# 索引操作符让我们可以从序列中抓取一个特定项目。
# 切片操作符让我们能够获取序列的一个切片，即一部分序列。

