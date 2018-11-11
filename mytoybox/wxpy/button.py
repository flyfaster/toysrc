#!/usr/bin/env python3

import wx
import platform

class ButtonFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, 'Button Example',
                          size=(600, 300))
        panel = wx.Panel(self, -1)
        self.hello_button = wx.Button(panel, -1, "Hello", pos=(50, 20))
        self.Bind(wx.EVT_BUTTON, self.OnClick, self.hello_button)
        self.hello_button.SetDefault()
        self.exit_button = wx.Button(panel, -1, "Exit", pos=(50, 20))
        self.Bind(wx.EVT_BUTTON, self.OnExit, self.exit_button)
        
        lbl = wx.StaticText(panel,
                            label="python " + platform.python_version() + " on " + platform.system() + " " + wx.__file__)
        
        bagSizer    = wx.GridBagSizer(hgap=5, vgap=5)
        bagSizer.Add(self.hello_button, pos=(0,0),
                     flag=wx.EXPAND|wx.ALL,
                     border=1)
        bagSizer.Add(self.exit_button, pos=(0,1),
                     flag=wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_CENTER_HORIZONTAL,
                     border=1)      
        bagSizer.Add(lbl, pos=(1,0), span=(1, 3),
                     flag=wx.EXPAND|wx.ALL,
                     border=1)
        bagSizer.AddGrowableCol(2, 0)
        panel.SetSizerAndFit(bagSizer)

    def OnClick(self, event):
        self.hello_button.SetLabel("Clicked")
        
    def OnExit(self, event):
        self.Close(True)
        
if __name__ == '__main__':
    app = wx.App()
    frame = ButtonFrame()
    frame.Show()
    app.MainLoop()
