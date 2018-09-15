#!/usr/bin/env python
# coding=utf-8

print "---1---"
myList=[[0,1],[2,3],[1,5],[1,4]]
myArray = [s for s in myList if s[0] == 1]
print myArray

print "---2---"
myArray = [s for s in myList if s[0] == 9]
print myArray
if not myArray:
    print "no items"
