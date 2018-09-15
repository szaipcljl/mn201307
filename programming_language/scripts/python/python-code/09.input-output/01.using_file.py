#!/usr/bin/env python
# coding=utf-8


# Filename:using_file.py

poem = '''\
Programing is fun
When the work is done
if you wanna make your work also fun:
    use Python!
'''

f = file('poem.txt', 'w') #open for 'w'riting
f.write(poem) #write text to file
f.close() #close the file


f = file('poem.txt')
# if no mode is specified, 'r'ead mode is assumed by default
while True:
    line = f.readline()
    if len(line) == 0: # Zero length indicate EOF
        break
    print line,
    # Notice comma to avoid automatic newline added by Python
f.close() #close the file

# note:
# 首先，我们通过指明我们希望打开的文件和模式来创建一个file类的实例。模式可以为读模式
# （'r'）、写模式（'w'）或追加模式（'a'）。
# help(file)
# 使用readline方法读文件的每一行。这个方法返回包括行末换行符
# 的一个完整行。
