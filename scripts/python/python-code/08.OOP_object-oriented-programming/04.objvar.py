#!/usr/bin/env python
# coding=utf-8

# Filename:objvar.py

class Person:
    '''Represents a person'''
    population = 0

    def __init__(self, name):
        '''Initializes the person's data.'''
        self.name = name
        print '(Initializing %s)' % self.name

        #When this Person is created, he/she
        #adds to the population
        Person.population += 1

    def __del__(self):
        '''I am dying.'''
        print '%s says bye.' % self.name
        Person.population -= 1

        if Person.population == 0:
            print 'i am the last one'
        else:
            print 'there are still %d people left.' % Person.population

    def sayHi(self):
        '''Greeting by the person.

        Really, that's all it does.'''
        print 'Hi, my name is %s.' % self.name

    def howMany(self):
        '''Prints the current popultion'''
        if Person.population == 1:
            print 'I am the only person here.'
        else:
            print 'We have %d person here.' % Person.population

chris = Person('Chris')
chris.sayHi()
chris.howMany()

kalam=Person('Kalam')
kalam.sayHi()
kalam.howMany()

chris.sayHi()
chris.howMany()

print Person.__doc__
print Person.sayHi.__doc__

# note:
# 有两种类型的 域 ——类的变量和对象的变量，它们根据是类还是对象 拥有 这个变量而区分。
# 类的变量 由一个类的所有对象（实例）共享使用。只有一个类变量的拷贝，所以当某个对象
# 对类的变量做了改动的时候，这个改动会反映到所有其他的实例上。
# 对象的变量 由类的每个对象/实例拥有。因此每个对象有自己对这个域的一份拷贝，即它们不
# 是共享的，在同一个类的不同实例中，虽然对象的变量有相同的名称，但是是互不相关的。

#
# population属于Person
# 类，因此是一个类的变量。name变量属于对象（它使用self赋值）因此是对象的变量
#
# docstring对于类和方法同样有用。

#在运行时使用Person.__doc__和Person.sayHi.__doc__来分别访问类与方法的文档字符串

#
# Python中所有的类成员（包括数据成员）都是 公共的 ，所有的方法都是 有效的 。
# 只有一个例外：如果你使用的数据成员名称以 双下划线前缀 比如__privatevar，Python的名称
# 管理体系会有效地把它作为私有变量
#
