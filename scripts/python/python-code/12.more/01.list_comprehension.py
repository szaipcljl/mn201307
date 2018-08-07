#!/usr/bin/env python
# coding=utf-8

# Filename: list_comprehension.py

listone = [2, 3, 4]
listtwo = [2*i for i in listone if i > 2]
print listtwo


# note:
# 通过列表综合，可以从一个已有的列表导出一个新的列表。例如，你有一个数的列表，而你想
# 要得到一个对应的列表，使其中所有大于2的数都是原来的2倍。
#
# 为满足条件（if i > 2）的数指定了一个操作（2*i），从而导出一个新的列表。
# 在很多时候，我们都是使用循环来处理列表中的每一个元素，
