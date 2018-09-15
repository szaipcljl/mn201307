#coding=utf-8

# icon : 保存项目使用的图片资源
# wx_main.py : 项目入口文件，运行此文件可以看见效果。
# loginFrame.py：登录的界面的Frame定义绘制文件
# contentFrame.py：登录成功之后的界面Frame定义绘制文件
# guiManager.py：界面创建和管理
# utils.py：工具类，其中定义了一个获取icon文件夹中文件全路径的工具函数
# xDialog.py：定义了有两项输入项的Dialog的样式


import wx
import guiManager as FrameManager

class MainAPP(wx.App):

    def OnInit(self):
        self.manager = FrameManager.GuiManager(self.UpdateUI)
        self.frame = self.manager.GetFrame(0)
        self.frame.Show()
        return True

    def UpdateUI(self, type):
        self.frame.Show(False)
        self.frame = self.manager.GetFrame(type)
        self.frame.Show(True)

def main():
    app = MainAPP()
    app.MainLoop()

if __name__ == '__main__':
    main()
