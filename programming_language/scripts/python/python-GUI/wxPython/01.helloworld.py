#!/usr/bin/env python
# coding=utf-8

# First things, first. Import the wxPython package.
#import wxversion
#wxversion.select('3.0')
import wx

# Next, create an application object.
app = wx.App()

# Then a frame.
frm = wx.Frame(None, title="Hello World")

# Show it.
frm.Show()

# Start the event loop.
app.MainLoop()
