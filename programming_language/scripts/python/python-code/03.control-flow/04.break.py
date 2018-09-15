#!/usr/bin/env python
# coding=utf-8

# Filename: break.py

while True:
    s = raw_input('Enter something : ')
    if s == 'quit':
        break
    print 'Length of the string is', len(s)

print 'Done'

# note: 输入字符串的长度通过内建的len函数取得
# break 语句对于for循环也有效
