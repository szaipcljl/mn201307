#!/usr/bin/env python
# coding=utf-8
class Dog:

    kind = 'canine'         # class variable shared by all instances

    def __init__(self, name):
        self.name = name    # instance variable unique to each instance

d = Dog('Fido')
e = Dog('Buddy')

print d.kind                  # shared by all dogs
print e.kind                  # shared by all dogs
print d.name                  # unique to d
print e.name                  # unique to e

#the tricks list in the following code should not be used as a class
#variable because just a single list would be shared by all Dog instances:
class Dog2:

    tricks = []             # mistaken use of a class variable

    def __init__(self, name):
        self.name = name

    def add_trick(self, trick):
        self.tricks.append(trick)

f = Dog2('Fido')
g = Dog2('Buddy')
f.add_trick('roll over')
g.add_trick('play dead')
print f.tricks                # unexpectedly shared by all dogs



#Correct design of the class should use an instance variable instead:
class Dog3:

    def __init__(self, name):
        self.name = name
        self.tricks = []    # creates a new empty list for each dog

    def add_trick(self, trick):
        self.tricks.append(trick)

d = Dog3('Fido')
e = Dog3('Buddy')
d.add_trick('roll over')
e.add_trick('play dead')
print d.tricks
print e.tricks

