#!/usr/bin/env python
# coding=utf-8

#
#Basic array operations
#

myList=[1,2,3,4,5,6]

#The first thing that we tend to need to do is to scan through an array and
#examine values. For example, to find the maximum value (forgetting for a moment
#that there is a built-in max function)  you could use:
m=0
for e in myList:
    if m<e:
        m=e
print m


#In most cases arrays are accessed by index and you can do this in Python:
#using range to generate the sequence 0,1, and so on up to the length of myList.
m=0
for i in range(len(myList)):
    if m<myList[i]:
        m=myList[i]
print m

#return not the maximum element but its index position in the list
m=0
mi=0
for i in range(len(myList)):
    if m<myList[i]:
        m=myList[i]
        mi=i
print mi


#or you could use the non-indexed loop and the index method:
m=0
for e in myList:
    if m<e:
        m=e
mi=myList.index(m)
print mi
