#!/usr/bin/env python
# coding=utf-8

import wx

class my_frame(wx.Frame):
    """We simple derive a new class of Frame"""
    def __init__(self,parent, title):
        wx.Frame.__init__(self, parent, title=title,size=(300,100))
        self.control = wx.TextCtrl(self, style=wx.TE_MULTILINE)
        self.Show(True)

app = wx.App(False)
frame = my_frame(None,'Small edior')
app.MainLoop()

# note:
# 输入多行文字wx.TextCtrl.
# 继承来自wx.Frame的__init__方法。声明一个wx.TextCtrl控件
