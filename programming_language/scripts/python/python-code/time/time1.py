#!/usr/bin/env python
# coding=utf-8

import time
# 时间转换
date_str = "2016-11-30 13:53:59"

struct_time = time.strptime(date_str, "%Y-%m-%d %H:%M:%S")
print "retured tuple: %s " % struct_time

t=int(time.mktime(time.strptime(date_str, "%Y-%m-%d %H:%M:%S")))
print t


# notes:
# Python time strptime() 函数根据指定的格式把一个时间字符串解析为时间元组。
# Python time mktime() 函数执行与gmtime(), localtime()相反的操作，它接收
# struct_time对象作为参数，返回用秒数来表示时间的浮点数。
