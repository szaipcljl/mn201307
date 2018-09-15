#!/usr/bin/env python
# coding=utf-8

class Sample:
    name = ''
    average = 0.0
    values = None # list cannot be initialized here!


s1 = Sample()
s1.name = "sample 1"
s1.values = []
s1.values.append(1)
s1.values.append(2)
s1.values.append(3)

s2 = Sample()
s2.name = "sample 2"
s2.values = []
s2.values.append(4)
s2.values.append([4,7])

for v in s1.values:   # prints 1,2,3 --> OK.
    print v
print "***"
for v in s2.values:   # prints 4 --> OK.
    print v
