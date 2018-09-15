#!/usr/bin/env python
# coding=utf-8

#
#List basics
#

#Python doesn't have a native array data structure, but it has the list which is
#much more general and can be used as a multidimensional array quite easily.
#you can add and delete elements from the list at any time.


#To define a list you simply write a comma separated list of items in square
#brackets:
myList=[1,2,3,4,5,6]

print "---1---"
#indexes start from 0; display the third element:
print myList[2]

#Similarly to change the third element you can assign directly to it:
print "---2---"
myList[2]=100
print myList[2]

#a sublist from the third element to the fifth i.e. from myList[2] to myList[4].
#notice that the final element specified i.e. [5] is not included in the slice.
print "---3---"
print myList[2:5]

#Also notice that you can leave out either of the start and end indexes and they
#will be assumed to have their maximum possible value.
# the list from List[5] to the end of the list and.
print "---4---"
print myList[5:]

#the list up to and not including myList[5]
print "---5---"
print myList[:5]

#the entire list.
print "---6---"
print myList[:]

#List slicing is more or less the same as string slicing except that you can
#modify a slice. 
print "---7---"
myList[0:2]=[0,1]
print myList[0:2]
