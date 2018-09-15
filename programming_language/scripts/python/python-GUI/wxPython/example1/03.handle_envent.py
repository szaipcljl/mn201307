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
        menu_exit = filemenu.Append(wx.ID_EXIT, "Exit", "Termanate the program")
        filemenu.AppendSeparator()
        menu_about = filemenu.Append(wx.ID_ABOUT, "About", "Information about this program")#设置菜单的内容

        menuBar = wx.MenuBar()
        menuBar.Append(filemenu, u"设置")
        self.SetMenuBar(menuBar)#创建菜单条
        self.Show(True)

        # 弹出对话框
        self.Bind(wx.EVT_MENU, self.on_about, menu_about)
        self.Bind(wx.EVT_MENU, self.on_exit, menu_exit)#把出现的事件，同需要处理的函数连接起来

    def on_about(self,e):#about按钮的处理函数
        dlg = wx.MessageDialog(self,"A samll text editor", "About sample Editor",wx.OK)#创建一个对话框，有一个ok的按钮
        dlg.ShowModal()#显示对话框
        dlg.Destroy()#完成后，销毁它。

    def on_exit(self,e):
        self.Close(True)


app = wx.App(False)
frame = my_frame(None, 'Small edior')
app.MainLoop()

# note:
# (1) 设定事件，然后设定事件出现后应该执行什么操作，最后把事件和操作连接起来
