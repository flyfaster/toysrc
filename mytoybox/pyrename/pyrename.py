#!/usr/bin/env python
import os, sys, datetime, time
import platform
from stat import *
import argparse
import wx

enable_milliseconds = False

def gettimestr(last_modify, enable_milliseconds):
	
	shorttimestr = "{0:04}{1:02}{2:02}{3:02}{4:02}{5:02}".format(last_modify.year, 
																last_modify.month, 
																last_modify.day, 
																last_modify.hour, 
																last_modify.minute, 
																last_modify.second)
	timestr_value = shorttimestr
	if enable_milliseconds:
		timestr_value = "{0}.{1:03}".format(shorttimestr, last_modify.microsecond/1000)							
		
	return timestr_value
			
def renamefile(sourcefile):
	mode = os.stat(sourcefile)[ST_MODE]
	if not S_ISREG(mode):
		print(sourcefile + " is not regular file")
		return sourcefile + " is not regular file"
	
	last_modify = datetime.datetime.fromtimestamp(os.stat(sourcefile)[ST_MTIME])
	shorttimestr = gettimestr(last_modify, False)
	# skip file that already has time stamp in name
	if shorttimestr in sourcefile:
		print(sourcefile + " already has time stamp")
		return sourcefile + " already has time stamp"
	
	timestr = gettimestr(last_modify, enable_milliseconds)
	path, file = os.path.split(sourcefile)
	new_full_path = os.path.join(path, timestr + "-" + file)
	os.rename(sourcefile, new_full_path)
	print ('rename '+ sourcefile + ' to '+ new_full_path)
	return 'rename '+ sourcefile + ' to '+ new_full_path
	
def renametool (source_dir):
	for one_file in os.listdir(source_dir):
		pathname = os.path.join(source_dir, one_file)
		renamefile(pathname)
		
class MainFrame(wx.Frame):
    def __init__(self):
		wx.Frame.__init__(self, None, -1, 'Rename file (pid {})'.format(os.getpid()), size=(600, 300))
		panel = wx.Panel(self, -1)
		self.base_tc = wx.TextCtrl(panel)
		self.base_button = wx.Button(panel, -1, "Source File", pos=(50, 20))
		self.Bind(wx.EVT_BUTTON, self.OnChooseBase, self.base_button)
		
		self.target_tc = wx.TextCtrl(panel)
		self.target_button = wx.Button(panel, -1, "Source Dir", pos=(50, 20))
		self.Bind(wx.EVT_BUTTON, self.OnChooseTarget, self.target_button)
		
		self.find_button = wx.Button(panel, -1, "Rename", pos=(50, 20))
		self.Bind(wx.EVT_BUTTON, self.OnClick, self.find_button)
		self.find_button.SetDefault()
		self.exit_button = wx.Button(panel, -1, "Exit", pos=(50, 20))
		self.Bind(wx.EVT_BUTTON, self.OnExit, self.exit_button)
		
		self.resultlbl = wx.StaticText(panel, label="")
		lbl = wx.StaticText(panel, label="python " + platform.python_version() + " on " + platform.system() + " " + wx.__file__)
		
		bagSizer = wx.GridBagSizer(hgap=5, vgap=5)
		row = 0
		col = 0
		bagSizer.Add(self.base_button, pos=(row, 0), flag=wx.EXPAND | wx.ALL, border=1)
		bagSizer.Add(self.base_tc, pos=(row, 1), span=(1, 3), flag=wx.EXPAND | wx.ALL, border=1)
		row += 1
		bagSizer.Add(self.target_button, pos=(row, 0), flag=wx.EXPAND | wx.ALL, border=1)
		bagSizer.Add(self.target_tc, pos=(row, 1), span=(1, 3), flag=wx.EXPAND | wx.ALL, border=1)
		row += 1
		bagSizer.Add(self.find_button, pos=(row, 0), flag=wx.EXPAND | wx.ALL, border=1)
		bagSizer.Add(self.exit_button, pos=(row, 1), flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_CENTER_HORIZONTAL, border=1)
		row += 1
		bagSizer.Add(self.resultlbl, pos=(row, 0), span=(1, 3), flag=wx.EXPAND | wx.ALL, border=1)
		row += 1
		bagSizer.Add(lbl, pos=(row, 0), span=(1, 3), flag=wx.EXPAND | wx.ALL, border=1)
		bagSizer.AddGrowableCol(2, 0)
		panel.SetSizerAndFit(bagSizer)

    def OnChooseBase(self, event):
        with wx.FileDialog(self, "Open source file", wildcard="source files (*.*)|*",
                   style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST) as fileDialog:

            if fileDialog.ShowModal() == wx.ID_CANCEL:
                return
    
            pathname = fileDialog.GetPath()
            
            try:
                with open(pathname, 'r') as file:
                    self.base_tc.SetValue(pathname)
            except IOError:
                wx.LogError("Cannot open file '%s'." % pathname)
        pass

    def OnChooseTarget(self, event):
        with wx.DirDialog(self, "Open source directory",
                   style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST) as dirDialog:

            if dirDialog.ShowModal() != wx.ID_CANCEL:
                self.target_tc.SetValue(dirDialog.GetPath())
                    
    def OnClick(self, event):
    	if len(self.target_tc.GetValue()) + len(self.base_tc.GetValue()) < 1:
			dlg = wx.MessageDialog(self, "Must set source file or source directory ")
			dlg.ShowModal()
			return    		
        if len(self.base_tc.GetValue()) > 0:
        	if not os.path.exists(self.base_tc.GetValue()):
	            dlg = wx.MessageDialog(self, "source file does not exist - " + self.base_tc.GetValue())
	            dlg.ShowModal()
	            return
	        else:
	        	status = renamefile(self.base_tc.GetValue())
	        	self.resultlbl.SetLabel(status)
	        	
    	if len(self.target_tc.GetValue()) > 0:
	        if not os.path.exists(self.target_tc.GetValue()):
	            dlg = wx.MessageDialog(self, "Source directory does not exist - " + self.target_tc.GetValue())
	            dlg.ShowModal()            
	            return
	        else:
	        	renametool(self.target_tc.GetValue())

    def OnExit(self, event):
        self.Close(True)

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Accepted arguments')
	parser.add_argument('--ms', help='enable millisecond in file name', required=False)
	parser.add_argument('--source', help='path of source file to be renamed', required=False)
	args = vars(parser.parse_args())
	
	if args['ms'] is not None:
		enable_milliseconds = True
	else:
		enable_milliseconds = False
	
	if args['source'] is None:
		app = wx.App()
		frame = MainFrame()
		frame.Show()
		app.MainLoop()
		sys.exit(0)

	if os.path.isfile(args['source']):
		renamefile(args['source'])
	else:
		renametool(args['source'])