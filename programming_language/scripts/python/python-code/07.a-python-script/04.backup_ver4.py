#!/usr/bin/env python
# coding=utf-8

# Filename: backup_ver4.py

import os
import time

# 1. The files and directories to be backed up are specified in a list.
source = ['/home/chris/byte', '/home/chris/bin']
# If you are using Windows, use source = [r'C:\Documents', r'D:\Work'] or something like that

# 2. The backup must be stored in a main backup directory
target_dir = '/home/chris/backup/' # Remember to change this to what you will be using

# 3. The files are backed up into a zip file.
# 4. The current day is the name of the subdirectory in the main directory
today = target_dir + time.strftime('%Y%m%d')
# The current time is the name of the zip archive
now = time.strftime('%H%M%S')

# Take a comment from the user to create the name of the zip file
comment = raw_input('Enter a comment --> ')
if len(comment) == 0: # check if a comment was entered
    target = today + os.sep + now + '.zip'
else:
    target = today + os.sep + now + '_' + \
            comment.replace(' ', '_') + '.zip'
    # Notice the backslash!

# Create the subdirectory if it isn't already there
if not os.path.exists(today):
    os.mkdir(today) # make directory
    print 'Successfully created directory', today

# 5. We use the zip command (in Unix/Linux) to put the files in a zip archive
zip_command = "zip -qr '%s' %s" % (target, ' '.join(source))

# Run the backup
if os.system(zip_command) == 0:
    print 'Successful backup to', target
else:
    print 'Backup FAILED'

# note:
# 我们使用raw_input函数得到
# 用户的注释，然后通过len函数找出输入的长度以检验用户是否确实输入了什么东西。如果用
# 户只是按了回车（比如这只是一个惯例备份，没有做什么特别的修改），那么我们就如之前那
# 样继续操作

# 如果提供了注释，那么它会被附加到zip归档名，就在.zip扩展名之前。注意我们把注释
# 中的空格替换成下划线——这是因为处理这样的文件名要容易得多。

# 进一步优化:
# 在程序中包含 交互 程度——你可以用-v选项来使你的程序更具交互性
# 通过sys.argv列表来获取它们，然后我们可以使用list类提供的extend方法把它们加到source列表中去。
# 使用tar命令替代zip命令, 结合使用tar和gzip 命令的时候，备份会更快更小。
# 命令字符串现在将称为：
# tar = 'tar -cvzf %s %s -X /home/swaroop/excludes.txt' % (target, ' '.join(srcdir))
# ● -c表示创建一个归档。
# ● -v表示交互，即命令更具交互性。
# ● -z表示使用gzip滤波器。
# ● -f表示强迫创建归档，即如果已经有一个同名文件, 它会被替换。
# ● -X表示含在指定文件名列表中的文件会被排除在备份之外。例如，你可以在文件中指定
#    *~，从而不让备份包括所有以~结尾的文件。
#
# 最理想的创建这些归档的方法是分别使用zipfile和tarfile。它们是Python标准库的一部分，可以
# 供你使用。使用这些库就避免了使用os.system这个不推荐使用的函数，它容易引发严重的错误。



