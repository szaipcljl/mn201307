#!/usr/bin/env python
#coding=utf-8
import wx
import time
import os

class MyApp(wx.App):

    def __init__(self):
        wx.App.__init__(self)

    def OnInit(self):
        self.Frame = wx.Frame(parent=None,title = "我的第一个GUI程序",pos = (350,200),size = (1000,800))
        self.Frame.SetMaxSize((1000,800))
        self.Frame.SetMinSize((1000,800))
        self.SetTopWindow(self.Frame)
        self.panel = wx.Panel(self.Frame,-1)
        self.Set_Ui_text()
        self.Set_Button()
        self.Set_Test_Ctrl()
        self.Set_Image()
        self.Set_Menu()
        self.Set_Radio_Box()
        self.Set_List_Box()
        #image1 = wx.Image("timg.bmp")
        #mage = wx.StaticBitmap(self.panel,-1,wx.BitmapFromImage(image1))
        sizer =self.Set_Sizer()
        self.Frame.SetSizer(sizer)
        #self.Frame.Fit()
        self.Frame.Show()

        return True

    def Set_Sizer(self):
        sizer = wx.GridSizer(rows = 4,cols = 4 ,hgap = 5,vgap = 5)
        #sizer.Add(self.button)
        sizer.Add(self.inputext)
        #sizer.Add(mage)
        sizer.Add(self.radiobox1)
        sizer.Add(self.radiobox2)
        sizer.Add(self.listbox1)
        sizer.Add(self.listbox2)
        sizer.Add(self.listbox3)
        sizer.Add(self.listbox4)
        sizer.Add(self.gauge1)
        sizer.Add(self.slider)
        sizer.Add(self.button1)

        return sizer

    def Set_Ui_text(self):
        self.label = wx.StaticText(self.panel, label="相关测试",size=(-1, 50))
        font = wx.Font(18, wx.ROMAN, wx.NORMAL, wx.NORMAL)
        self.label.SetFont(font)
        self.label.SetBackgroundColour("balck")
        self.label.SetForegroundColour("red")

    def Set_Button(self):
        self.button = wx.Button(self.panel, -1, "按钮1", pos=(300,50),size=(-1, -1))
        font = wx.Font(18, wx.ROMAN, wx.NORMAL, wx.NORMAL)
        self.button.SetFont(font)
        self.Bind(wx.EVT_BUTTON, self.two_play, self.button)

    def Set_Test_Ctrl(self):
        self.inputext = wx.TextCtrl(self.panel, -1, "请您输入版本路径：", size=(200, 40))
        self.inputext.Bind(wx.EVT_LEFT_DOWN,self.open_File)
        font = wx.Font(18, wx.ROMAN, wx.NORMAL, wx.NORMAL)
        self.inputext.SetFont(font)
        #self.inputext.SetInsertionPoint(0)

    def Set_Image(self):
        bmp = wx.Image("timg.bmp", wx.BITMAP_TYPE_BMP).ConvertToBitmap()
        self.button1 = wx.BitmapButton(self.panel, -1, bmp, size=(-1, -1))
        self.button1.Bind(wx.EVT_LEFT_DOWN, self.one_play)

    def Set_Menu(self):
        menubar = wx.MenuBar()
        menu1 = wx.Menu()
        menu3 = wx.Menu()
        bbu1 = menu1.Append(-1, "B2200")
        bbu2 = menu1.Append(-1, "B3200")
        cc1 = menu3.Append(-1, "cc1")
        cc2 = menu3.Append(-1, "cc2")
        cc3 = menu3.Append(-1, "cc3")
        menu1.AppendMenu(-1, "CC", menu3)
        self.Bind(wx.EVT_MENU, self.one_play, bbu1)
        self.Bind(wx.EVT_MENU, self.one_play, bbu2)
        menubar.Append(menu1, "菜单")
        menu2 = wx.Menu()
        menu2.AppendSeparator()
        rru1 = menu2.Append(-1, "R2254")
        rru2 = menu2.Append(-1, "R2252")
        self.Bind(wx.EVT_MENU, self.one_play, rru1)
        self.Bind(wx.EVT_MENU, self.one_play, rru2)
        menubar.Append(menu2, "工具")
        # 设置弹出菜单
        self.Menu4 = wx.Menu()
        self.Menu4.Append(-1, "1")
        self.Menu4.Append(-1, "2")
        self.Menu4.Append(-1, "3")
        self.Menu4.Append(-1, "4")
        self.Bind(wx.EVT_CONTEXT_MENU, self.Menu4_Test)
        self.Frame.SetMenuBar(menubar)
        print menubar.GetLabelTop(0)
        print menubar.FindMenu("BBU")
        # 状态栏
        status = self.Frame.CreateStatusBar()
        status.SetStatusText("write bu zhouqiang")

    def Menu4_Test(self,event):
        pos = event.GetPosition()
        print pos
        pos = self.panel.ScreenToClient(pos)
        print pos
        self.panel.PopupMenu(self.Menu4,pos)

    def Set_Radio_Box(self):
        list1 = ["BPN2", "BPL1", "BPC"]
        list2 = ["RRU1", "RRU2", "RRU3"]
        self.radiobox1 = wx.RadioBox(self.panel, -1, "网管选择", (-1, -1), (200, 20), list1, 3, wx.RA_SPECIFY_COLS)
        self.radiobox2 = wx.RadioBox(self.panel, -1, "射频设备选择", (-1, -1), (200, 20), list2, 3, wx.RA_SPECIFY_ROWS)
        self.radiobox1.Bind(wx.EVT_RADIOBOX, self.four_play)
        self.radiobox2.Bind(wx.EVT_RADIOBOX, self.five_play)

    def Set_List_Box(self):
        list1 = ["BPN2", "BPL1", "BPC"]
        list2 = ["RRU1", "RRU2", "RRU3"]
        # ListBox类实例
        self.listbox1 = wx.ListBox(self.panel, -1, (-1, -1), (200, 60), list1, wx.LB_SINGLE)  # wx.LB_SINGLE只能选择单个
        self.listbox2 = wx.ListBox(self.panel, -1, (-1, -1), (200, 60), list2, wx.LB_MULTIPLE)  # 多选
        # CheckListBox类实例
        self.listbox3 = wx.CheckListBox(self.panel, -1, (-1, -1), (200, 60), list1)
        # Choice类实例
        self.listbox4 = wx.Choice(self.panel, -1, (-1, -1), (200, 40), list2)
        self.listbox4.Bind(wx.EVT_CHOICE, self.three_play)
        # 进度条展示
        self.gauge1 = wx.Gauge(self.panel, -1, 100, (-1, -1), (200, 60))
        self.value = 1
        self.gauge1.SetValue(self.value)
        # 将wx空闲的事件绑定到进度条上
        self.Bind(wx.EVT_IDLE, self.Gauge_Test)
        # 滑块
        self.slider = wx.Slider(self.panel, -1, 10, 10, 100, (-1, -1), (200, 60))
        self.slider.Bind(wx.EVT_SCROLL, self.Slider_Test)


    def one_play(self,event):
        print "这是一个回调函数"

    def Gauge_Test(self,event):
        if self.value < 100:
            self.value += 1
            time.sleep(0.3)
            self.gauge1.SetValue(self.value)

    def Slider_Test(self,event):
        value = self.slider.GetValue()
        print "now value is:",value

    def open_File(self,event):
        file = wx.FileDialog(None,"choose you file",os.getcwd(),"","",wx.FD_OPEN)
        if file.ShowModal() == wx.ID_OK:
            i = file.GetPath()
            print i
        file.Destroy()
        self.inputext.SetLabel(i)

    def two_play(self,vent):
        dlg = wx.ProgressDialog("执行进度条","进行中",100,style = wx.PD_CAN_ABORT)
        value = 0
        while value < 100:
            value += 1
            time.sleep(0.1)
            dlg.Update(value)
        else:
            dlg.Destroy()

    def three_play(self,event):
        wx.MessageBox("这是一个回调函数","123456")

    def four_play(self,event):
        data = wx.GetTextFromUser("plase input you dota:","one and one")
        print data

    def five_play(self,event):
        list1 = ('1','2','3','4','9')
        data2 = wx.GetSingleChoice("what do you what:","12345",list1)
        print data2

if __name__=="__main__":
    MyApp().MainLoop()
