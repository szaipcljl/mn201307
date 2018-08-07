#!/usr/bin/env python
# coding=utf-8

# Filename: finally.py

import time

poem = '''\
Programing is fun
When the work is done
if you wanna make your work also fun:
    use Python!
'''

f = file('poem.txt', 'w') #open for 'w'riting
f.write(poem) #write text to file
f.close() #close the file

try:
    f = file('poem.txt')
    while True: # our usual file-reading idiom
        line = f.readline()
        if len(line) == 0:
            break
        time.sleep(2)
        print line,
finally:
    f.close()
    print 'Cleaning up...closed the file'



# note:
# try..finally
# 在程序运行的时候，
# 按Ctrl-c中断/取消程序。
# 我们可以观察到KeyboardInterrupt异常被触发，程序退出。但是在程序退出之前，finally从句仍
# 然被执行，把文件关闭
