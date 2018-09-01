#!/usr/bin/env python
# coding=utf-8

import cv2
cv2.namedWindow('testcamera', cv2.WINDOW_AUTOSIZE)

capture = cv2.VideoCapture(0)
print 'capture is opened:', capture.isOpened()
num = 0

while 1:
    ret, img = capture.read()
    cv2.imshow('testcamera', img)
    key = cv2.waitKey(1)
    num += 1
    print 'key=', key, 'num=', num #debug

    if key==27:#<ESC>
        break

capture.release()
cv2.destroyAllWindows()


# notes:
# 用法：cv2.namedWindow('窗口标题',默认参数)
#
# 默认参数：cv2.WINDOW_AUTOSIZE+cv2.WINDOW_KEEPRATIO+cv2.WINDOW_GUI_EXPANDED)
#
#     参数：
#
#     cv2.WINDOW_NORMAL     窗口大小可改变
#
#     cv2.WINDOW_AUTOSIZE   窗口大小不可改变
#
#     cv2.WINDOW_FREERATIO  自适应比例
#
#     cv2.WINDOW_KEEPRATIO  保持比例饿
#
#     cv2.WINDOW_GUI_NORMAL
#
#     cv2.WINDOW_GUI_EXPANDED
#
# 用法：cv2.imshow('窗口标题', image)，如果前面没有cv2.namedWindow，就自动先执行一个cv2.namedWindow。

# 1、cap = cv2.VideoCapture(0)
#
# VideoCapture()中参数是0，表示打开笔记本的内置摄像头; 参数是视频文件路径则打开视频，如cap
# = cv2.VideoCapture(“../test.avi”)

#
# 2、ret,frame = cap.read()
#
# cap.read()按帧读取视频，ret,frame是获cap.read()方法的两个返回值。其中ret是布尔值，如果读取帧
# 是正确的则返回True，如果文件读取到结尾，它的返回值就为False。frame就是每一帧的图像，是个三维矩阵。
#

#
# 3、cv2.waitKey(1)，waitKey（）方法本身表示等待键盘输入，
#
# 参数是1，表示延时1ms切换到下一帧图像，对于视频而言；
# 参数为0，如cv2.waitKey(0)只显示当前帧图像，相当于视频暂停,；
# 参数过大如cv2.waitKey(1000)，会因为延时过久而卡顿感觉到卡顿。
#
# key 得到的是键盘输入的ASCII码，esc键对应的ASCII码是27，即当按esc键是if条件句成立

#
# 4、调用release()释放摄像头，调用destroyAllWindows()关闭所有图像窗口。
#







