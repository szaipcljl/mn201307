#!/usr/bin/env python
# coding=utf-8

#Class objects
#Class objects support two kinds of operations: attribute references and instantiation.
class MyClass:
    """A simple example class"""
    def __init__(self):
        self.data = []
    i = 12345

    def f(self):
        return 'hello world'
#__doc__ is also a valid attribute, returning the docstring belonging to the class: "A simple example class".

#When a class defines an __init__() method, class instantiation automatically invokes
#__init__() for the newly-created class instance.


#Class instantiation uses function notation.
#Just pretend that the class object is a parameterless function
#that returns a new instance of the class:
x = MyClass()
print x.i
print x.__doc__



#the __init__() method may have arguments for greater flexibility. In that case,
#arguments given to the class instantiation operator are passed on to
#__init__().
class Complex:
    def __init__(self, realpart, imagpart):
        self.r = realpart
        self.i = imagpart

y = Complex(3.0, -4.5)
print y.r, y.i

#
#Instance Objects
#
#Data attributes need not be declared
y.counter = 1
while y.counter < 10:
    y.counter = y.counter * 2
print(y.counter)
del y.counter

#Method Objects
#Usually, a method is called right after it is bound:
print x.f()
#x.f is a method object, and can be stored away and called at a later time.
xf = x.f
print(xf())
