#!/usr/bin/env python
# coding=utf-8

# Filename: using_dict.py

# 'ab' is short for 'a'ddress'b'ook

ab = { 'Swaroop' : 'swaroopch@byteofpython.info',
        'Larry' : 'larry@wall.org',
        'Matsumoto' : 'matz@ruby-lang.org',
        'Spammer' : 'spammer@hotmail.com'
        }

print "Swaroop's address is %s" % ab['Swaroop']

# Adding a key/value pair
ab['Guido'] = 'guido@python.org'

# Deleting a key/value pair
del ab['Spammer']

print '\nThere are %d contacts in the address-book\n' % len(ab)

for name, address in ab.items():
    print 'Contact %s at %s' % (name, address)

if 'Guido' in ab: # OR ab.has_key('Guido')
    print "\nGuido's address is %s" % ab['Guido']

# note:
# 字典(dictionary)是dict类的实例/对象.
# 键必须是唯一的
# 只能使用不可变的对象（比如字符串）作为字典的键，
# 可以把不可变或可变的对象作为字典的值。
# 只使用简单的对象作为键。
# 键值对在字典中标记方式：d = {key1 : value1, key2 : value2 }。
# 键/值对用冒号分割，各个对用逗号分割，所有这些都包括在花括号中。
# 字典中的键/值对是没有顺序的。想要一个特定的顺序，在使用前自己对它们排序。
# 使用字典的items方法，来使用字典中的每个键/值对。 这会返回一个元组的列
# 表，其中每个元组都包含一对项目——键与对应的值
# 使用in操作符来检验一个键/值对是否存在，或者使用dict类的has_key方法。
# help(dict)来查看dict类的完整方法列表
