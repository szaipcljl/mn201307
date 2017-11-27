#!/usr/bin/env python
# coding=utf-8

# If your file is not too large, you can read it into a string, and just use that
# (easier and often faster than reading and checking line per line):
if 'string' in open('example.txt').read():
    print("true")


#Another trick: you can alleviate the possible memory problems by using
#mmap.mmap() to create a "string-like" object that uses the underlying
#file (instead of reading the whole file in memory):
import mmap

f = open('example.txt')
s = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
if s.find('blabla') != -1:
    print('true')
