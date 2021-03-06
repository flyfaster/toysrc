#!/usr/bin/env python3

"""
brew install wxpython does not work, but pip works fine on macos High Sierra.
sh-3.2$ sw_vers
ProductName:    Mac OS X
ProductVersion: 10.13.6
BuildVersion:   17G3025

pip3 install -U wxPython
"""

import os
import platform
import wx
import wx.grid
import subprocess
import threading
import datetime
import time

class TimerEvent(wx.PyCommandEvent):
    def __init__(self, etype, eid, value=None):
        wx.PyCommandEvent.__init__(self, etype, eid)
        self._value = value

    def GetValue(self):
        """Returns the value from the event.
        @return: the value of this event
        """
        return self._value
    
class Timer(threading.Thread):
    """Call a function after a specified number of seconds:
 
            t = Timer(30.0, f, args=None, kwargs=None)
            t.start()
            t.cancel()     # stop the timer's action if it's still waiting
 
    """
 
    def __init__(self, interval, function, args=None, kwargs=None):
        threading.Thread.__init__(self)
        self.interval = interval
        self.function = function
        self.args = args if args is not None else []
        self.kwargs = kwargs if kwargs is not None else {}
        self.finished = threading.Event()
 
    def cancel(self):
        """Stop the timer if it hasn't finished yet."""
        self.finished.set()
 
    def run(self):
        while not self.finished.is_set():
            self.finished.wait(self.interval)
            self.function(*self.args, **self.kwargs)
        self.finished.set()
        
class MainWindow(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(800,400))
        self.grid = wx.grid.Grid(self, -1)
        
        column_labels = self.get_column_labels()
        self.grid.CreateGrid(1, len(column_labels))
        for i in range(len(column_labels)):
            self.grid.SetColLabelValue(i, column_labels[i])

        selectBtn = wx.Button(self, label="Exit")
        selectBtn.Bind(wx.EVT_BUTTON, self.OnExit)
        
        version_txt = "pid {} python {} on {} ".format(os.getpid(), platform.python_version(), platform.system()) + wx.__file__
        self.lbl = wx.StaticText(self,
                            label=version_txt)
        
        bagSizer    = wx.GridBagSizer(hgap=5, vgap=5)
        bagSizer.Add(self.grid, pos=(0,0),
                     flag=wx.EXPAND|wx.ALL,
                     border=1)
        bagSizer.Add(self.lbl, pos=(1,0),
             flag=wx.EXPAND|wx.ALL,
             border=0)

        bagSizer.Add(selectBtn, pos=(2,0),
                     flag=wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_CENTER_HORIZONTAL,
                     border=1)      
        bagSizer.AddGrowableCol(0, 0)
        bagSizer.AddGrowableRow(0, 0)
        self.SetSizerAndFit(bagSizer)
        self.CreateStatusBar() # A StatusBar in the bottom of the window

        # Setting up the menu.
        filemenu= wx.Menu()

        # wx.ID_ABOUT and wx.ID_EXIT are standard ids provided by wxWidgets.
        menuAbout = filemenu.Append(wx.ID_ABOUT, "&About"," Information about this program")
        menuExit = filemenu.Append(wx.ID_EXIT,"E&xit"," Terminate the program")

        # Creating the menubar.
        menuBar = wx.MenuBar()
        menuBar.Append(filemenu,"&File") # Adding the "filemenu" to the MenuBar
        self.SetMenuBar(menuBar)  # Adding the MenuBar to the Frame content.

        # Set events.
        self.Bind(wx.EVT_MENU, self.OnAbout, menuAbout)
        self.Bind(wx.EVT_MENU, self.OnExit, menuExit)
        self.EVT_WORK_DONE_TYPE = wx.NewEventType()
        self.refreshthread = Timer(1.0, self.RefreshPorts)
        self.refreshthread.start()
        
        self.EVT_WORK_DONE = wx.PyEventBinder(self.EVT_WORK_DONE_TYPE, 1)
        self.Bind(self.EVT_WORK_DONE, self.DoRefreshPorts)
        self.RefreshPorts()
        
        self.Show(True)
        
    def get_column_labels(self):
        if platform.system() == 'Linux':
            str_list = 'Proto Recv-Q Send-Q LocalAddress ForeignAddress State PID/ProgramName'.split()
            cols = list(filter(None, str_list))  # remove empty strings
            return cols
        if platform.system() == 'Darwin':
            column_labels = 'COMMAND PID USER FD TYPE DEVICE SIZE/OFF NODE NAME'.split(' ')
            return column_labels
    
    def mac_get_ports(self):
        cmdstr = 'lsof -iTCP -sTCP:LISTEN -n -P'
        result = subprocess.run(cmdstr.split(' '), stdout=subprocess.PIPE)
        lines = result.stdout.decode('utf-8').splitlines()[1:]
        for i in range(len(lines)):
            if len(lines[i]) > 1:
                self.mac_add_line(i, lines[i])
        extra_rows = self.grid.GetNumberRows() - len(lines)
        if extra_rows > 0:
            self.grid.DeleteRows(len(lines), extra_rows)
            
    def linux_get_ports(self):
        cmdstr = 'netstat -tnlp'
        result = subprocess.run(cmdstr.split(' '), stdout=subprocess.PIPE)
        lines = result.stdout.decode('utf-8').splitlines()[1:]
        for i in range(len(lines)):
            if len(lines[i]) > 1:
                self.linux_add_line(i, lines[i])
        extra_rows = self.grid.GetNumberRows() - len(lines)
        if extra_rows > 0:
            self.grid.DeleteRows(len(lines), extra_rows)
            
    def mac_add_line(self, row, line):        
        str_list = line.split(' ')
        cols = list(filter(None, str_list))  # remove empty strings
        for col in range(self.grid.GetNumberCols()):
            if col >= len(cols):
                break
            self.SetCellValue(row, col, cols[col])
            
    def linux_add_line(self, row, line):        
        str_list = line.split(' ')
        cols = list(filter(None, str_list))  # remove empty strings
        for col in range(self.grid.GetNumberCols()):
            if col >= len(cols):
                break
            self.SetCellValue(row, col, cols[col])
            
    def UpdateStatus(self):
        #self.lbl.SetLabel(datetime.datetime.now().isoformat() + " pid {} python {} on {} ".format(os.getpid(), platform.python_version(), platform.system()) + wx.__file__)
        self.GetStatusBar().SetStatusText("Updates at " + datetime.datetime.now().isoformat())
    def DoRefreshPorts(self, event):
        if platform.system() == 'Linux':
            self.linux_get_ports()
            pass
        elif platform.system() == 'Darwin':
            self.mac_get_ports()
        self.UpdateStatus()
        pass

    def RefreshPorts(self):
        evt = TimerEvent(self.EVT_WORK_DONE_TYPE, -1)
        wx.PostEvent(self, evt)
        pass
    
    def SetCellValue(self, row, col, txt):
        insert_rows = row + 1 - self.grid.GetNumberRows()
        if insert_rows > 0:
            self.grid.AppendRows(insert_rows)
        self.grid.SetCellValue(row, col, txt)
        self.grid.SetReadOnly(row, col)
                            
    def OnAbout(self, e):
        dlg = wx.MessageDialog(self, "A tool to display open TCP ports", "About " + os.path.basename(__file__), wx.OK)
        dlg.ShowModal() 
        dlg.Destroy() 

    def OnExit(self, e):
        self.refreshthread.cancel()
        self.Unbind(self.EVT_WORK_DONE)
        time.sleep(0.3)
        self.Close(True)


if __name__ == '__main__':
    app = wx.App(False)
    frame = MainWindow(None, "Open TCP ports")
    app.MainLoop()
