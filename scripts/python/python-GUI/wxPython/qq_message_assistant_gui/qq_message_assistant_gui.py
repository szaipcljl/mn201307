#!/usr/bin/env python
# coding=utf-8

import wx
import wx.py.images as images
class MainFrame(wx.Frame):
    def __init__(self,parent,id):
        wx.Frame.__init__(self,parent,id,'QQ消息助手',size=(500,400))
        panel=wx.Panel(self)
        panel.SetBackgroundColour('White')
        #menubar
        ID_Q=101
        ID_SET=102
        ID_ABOUT=103
        menubar=wx.MenuBar()
        quitmenu=wx.Menu()
        menubar.Append(quitmenu,"退出")
        quitmenu.Append(ID_Q,"退出")
        setmenu=wx.Menu()
        menubar.Append(setmenu,"设置")
        setmenu.Append(ID_SET,"账号设置")
        aboutmenu=wx.Menu()
        menubar.Append(aboutmenu,"关于")
        aboutmenu.Append(ID_ABOUT,"作者信息")
        self.SetMenuBar(menubar)


        #statusbar
        statusbar=self.CreateStatusBar()
        statusbar.SetStatusText("版权所有：**  联系方式：f**@163.com")
        #ckecklistbox
        self.contect=['**1223','飞12']
        self.clb=wx.CheckListBox(panel,wx.NewId(),(5,30),(150,270),self.contect,wx.LB_ALWAYS_SB)
        #静态文本框：QQ联系人/QQ群
        text1=wx.StaticText(panel,wx.NewId(),"QQ联系人/QQ群",(10,5),(100,20),wx.ALIGN_LEFT)
        #发送文本框
        self.sendtext=wx.TextCtrl(panel,wx.NewId(),'输入发送内容',(180,10),(300,140),wx.TE_LEFT|wx.TE_MULTILINE)
        #发送按钮
        self.button=wx.Button(panel,wx.NewId(),"发送",(200,200),(50,30))
        self.Bind(wx.EVT_BUTTON,self.ONClick,self.button)

        wx.CheckBox(panel,wx.NewId(),'简单发送',(200,300),(100,-1))


    def ONClick(self,event):
        self.button.SetLabel("aaa")

if __name__=='__main__':
    app=wx.App()
    frame=MainFrame(parent=None,id=0)
    frame.Show()
    app.MainLoop()
