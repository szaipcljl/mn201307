#!/usr/bin/env python
# coding=utf-8

print "1.For loop from 0 to 2, therefore running 3 times.:"
for x in range(0, 3):
    print "We're on time %d" % (x)


print "2.While loop from 1 to infinity, therefore running forever.:"
#x = 1
#while True:
    #print "To infinity and beyond! We're getting close, on %d now!" % (x)
    #x += 1

print "3.Nested loops:"
for x in xrange(1, 5):
    for y in xrange(1, 5):
        print '%d * %d = %d' % (x, y, x*y)

print "4.Early exits:"
for x in xrange(3):
    if x == 1:
        break

print "5.For..Else:"
for x in xrange(3):
    print x
else:
    print 'Final x = %d' % (x)


print "6.Strings as an iterable:"
string = "Hello World"
for x in string:
    print x

print "7.Lists as an iterable:"
collection = ['hey', 5, 'd']
for x in collection:
    print x


print "8.Loop over Lists of lists:"
list_of_lists = [ [1, 2, 3], [4, 5, 6], [7, 8, 9]]
for list in list_of_lists:
    for x in list:
        print x
