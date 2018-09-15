#!/usr/bin/env python
# coding=utf-8

# Filename: backup_ver1.py

import os
import time

# 1. The files and directories to be backed up are specified in a list.
source = ['/home/chris/byte', '/home/chris/bin']
# If you are using Windows, use source = [r'C:\Documents', r'D:\Work'] or something like that

# 2. The backup must be stored in a main backup directory
target_dir = '/home/chris/backup/' # Remember to change this to what you will be using

# 3. The files are backed up into a zip file.
# 4. The name of the zip archive is the current date and time
target = target_dir + time.strftime('%Y%m%d%H%M%S') + '.zip'

# 5. We use the zip command (in Unix/Linux) to put the files in a zip archive
zip_command = "zip -qr '%s' %s" % (target, ' '.join(source))

# Run the backup
if os.system(zip_command) == 0:
    print 'Successful backup to', target
else:
    print 'Backup FAILED'

#note:
# 使用加法操作符来 级连 字符串，即把两个字符串连接在一起返回一个新的字符串
# zip命令有一些选项和参数。-q选项用来表示zip命令安静地工作。-r选项表示zip命令对目录递
# 归地工作，
# 字符串join方法把source列表转换为字符串
# 使用os.system函数 运行 命令
