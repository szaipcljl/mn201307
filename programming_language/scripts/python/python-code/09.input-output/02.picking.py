#!/usr/bin/env python
# coding=utf-8

# Filename:pickling.py

import cPickle as p
#import pickle as p

shoplistfile = 'shoplist.data'
#the nae of the file where we will store the object

shoplist = ['apple', 'mango', 'carrot']

#Write to the file
f = file(shoplistfile, 'w')
p.dump(shoplist, f) #dump the object to file
f.close()

del shoplist #remove the shoplist

# Read back from the storage
f = file(shoplistfile)
storedlist = p.load(f)
print storedlist

# note:
# 储存器
# Python提供一个标准的模块，称为pickle。使用它你可以在一个文件中储存任何Python对象，之
# 后你又可以把它完整无缺地取出来。这被称为 持久地 储存对象。
# 还有另一个模块称为cPickle，它的功能和pickle模块完全相同，只不过它是用C语言编写的，因
# 此要快得多（比pickle快1000倍）。你可以使用它们中的任一个，而我们在这里将使用cPickle模
# 块。记住，我们把这两个模块都简称为pickle模块。
#
# import..as语法。这是一种便利方法，以便于我们可以使用更短的模块名称。
#
# 为了在文件里储存一个对象，首先以写模式打开一个file对象，然后调用储存器模块的dump函
# 数，把对象储存到打开的文件中。这个过程称为 储存 。
# 接下来，我们使用pickle模块的load函数的返回来取回对象。这个过程称为 取储存
