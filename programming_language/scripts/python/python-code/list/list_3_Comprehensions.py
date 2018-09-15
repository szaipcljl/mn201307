#!/usr/bin/env python
# coding=utf-8

#uppose you want to create an array initialized to a particular value. Following
#the general array idiom in most languages you might write:

#myList=[]
#for i in range(10):
    #myList[i]=1

#only to discover that this doesn't work because you can't assign to a
#list element that doesn't already exist.

#One solution is to use the append method to add elements one by one:

#myList=[]
#for i in range(10):
    #myList.append(1)

#This works but it only works if you need to build up the list in this particular
#order - which most of the time you do. When the same situation arises in two-
#and multi-dimensioned arrays the problem often isn't as easy to solve with
#append, and there is a better way.

#When you create an array in other languages you can usually perform operations
#like:

    #a[i]=0
#without having to worry if a[i] already exists. In many cases the cost of
#this facility is that you have to declare the size of the array using a
#dimension statement of some sort. Python doesn't have such a facility
#because lists are dynamic and don't have to be "dimensioned". It is fairly
#easy to do the same job as a dimension statement, however, using a
#"comprehension".

#
#Comprehensions
#

#A comprehension is roughly speaking just an expression that specifies a sequence
#of values - think of it as a compact for loop. In Python a comprehension can be
#used to generate a list.

#This means that we can use a comprehension to initialize a list so that it has a
#predefined size. The simplest form of a list comprehension is

#[expression for variable in list]


print "---1---"
#For example, to create the list equivalent of a ten-element array you could
#write:
myList=[0 for i in range(10)]
print myList[:]

#Following this you have a ten-element list and you can write
#myList[i]=something
#without any worries - as long as i<10.

print "---2---"
#You can also use the for variable in the expression. For example:
myList=[i for i in range(10)]
print myList[:]

print "---3---"
myList=[i*i for i in range(10)]
print myList[:]
#The idea is that if you want to treat a list as an array then initializing it in
#this way can be thought of as the Python equivalent of dimensioning the array.



#
#Two dimensions
#
#As a list can contain any type of data there is no need to create a special
#two-dimensional data structure. All you have to do is store lists within lists -
#after all what is a two-dimensional array but a one-dimensional array of rows.

#In Python a 2x2 array is [[1,2],[3,4]] with the list [1,2] representing the
#first row and the list [3,4] representing the second row. You can use slicing to
#index the array in the usual way.
print "---4---"
myArray=[[1,2],[3,4]]
print myArray[0]
print myArray[0][1]

#As long as you build your arrays as nested lists in the way described then you
#can use slicing in the same way as you would array indexing. That is:
#myArray[i][j]
#is the i,jth element of the array.

#to do something with each element in myArray you might write:
print "---5---"
for i in range(len(myArray)):
    for j in range(len(myArray[i])):
        print myArray[i][j]

#Where len(myArray) us used to get the number of rows and len(myArray[i])) to get
#the number of elements in a row. Notice that there is no need for all of the
#rows to have the same number of elements, but if you are using a list as an
#array this is an assumption you need to enforce.


print "---6---"
#Notice that in the two-dimensional case the non-indexed for loop can also prove
#useful, but you cannot avoid a nested loop:
for row in myArray:
    for e in row:
        print e


#You can also use the index method to recover the i,j type index of an element.
#to print the row and column index of the element:
print "---7---"
for row in myArray:
    for e in row:
        print myArray.index(row),row.index(e)

#use a nested comprehension to create the list. For example, to create a 3x3
#matrix we could use:
print "---8---"
myArray=[[0 for j in range(3)] for i in range(3)]
print myArray
#the inner comprehension is just:
#[0 for j in range(3)]
#which creates a row, and then the outer comprehension just creates a list of rows.

print "---9---"
#use the index variables in the expression. For example:
myArray=[[i*j for j in range(3)] for i in range(3)]
print myArray

print "---10---"
#In general, if you want to work with an m by n array use:
m=4
n=5
myArray=[[0 for j in range(n)] for i in range(m)]
print myArray
#and everything should work as you would expect.


#In Python there are lots of clever ways of doing things that you generally
#wouldn't dream of in a lesser language. For example, a comprehension can be used
#to generate a list based on other lists. It is generally easy to get hold of the
#rows of a matrix:
#for row in myArray:
    #do something with row

#but getting at the columns as lists is much more difficult. However, with
#the help of a comprehension it is easy to get column j as a list:
#col=[row[j] for row in myArray]

#Using the same idea, if you want a transpose a matrix then usually you need to
#write two explicit for loops but to do the job in Python you can simply write:
#myArray= [[row[i] for row in myArray]
#          for i in range(len(myArray[0]))]
