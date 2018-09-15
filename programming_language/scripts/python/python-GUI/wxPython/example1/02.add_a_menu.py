#!/usr/bin/env python
# coding=utf-8

import wx

class my_frame(wx.Frame):
    """We simple derive a new class of Frame"""
    def __init__(self,parent, title):
        wx.Frame.__init__(self, parent, title=title,size=(300,200))
        self.control = wx.TextCtrl(self, style=wx.TE_MULTILINE,)
        self.Show(True)

        self.CreateStatusBar()#创建窗口底部的状态栏

        filemenu = wx.Menu()
        filemenu.Append(wx.ID_EXIT, "Exit", "Termanate the program")
        filemenu.AppendSeparator()
        filemenu.Append(wx.ID_ABOUT, "About", "Information about this program")#设置菜单的内容

        menuBar = wx.MenuBar()
        menuBar.Append(filemenu, u"设置")
        self.SetMenuBar(menuBar)#创建菜单条
        self.Show(True)

app = wx.App(False)
frame = my_frame(None, 'Small edior')
app.MainLoop()
