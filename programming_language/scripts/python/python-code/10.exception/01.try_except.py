#!/usr/bin/env python
# coding=utf-8


#Filename:try_except.py

import sys

try:
    s = raw_input('Enter something ->')
except EOFError:
    print '\mWhy did you do an EOF on me?'
    sys.exit() #exit the program
except:
    print '\nSome error /exceprion occured.'
    #here, we are not exiting the program

print 'Done'

# note:
# try..except

# 尝试读取用户的一段输入。按Ctrl-d，Python引发了一个称为EOFError的错误，
# 这个错误基本上意味着它发现一个不期望的 文件尾（由Ctrl-d表示）

# 我们把所有可能引发错误的语句放在try块中，然后在except从句/块中处理所有的错误和异常。
# except从句可以专门处理单一的错误或异常，或者一组包括在圆括号内的错误/异常。如果没有
# 给出错误或异常的名称，它会处理 所有的 错误和异常。对于每个try从句，至少都有一个相关
# 联的except从句

# 你还可以让try..catch块关联上一个else从句。当没有异常发生的时候，else从句将被执行。
