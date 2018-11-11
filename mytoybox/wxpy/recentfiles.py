#!/usr/bin/env python3
# Python program to find recent modified files
import os
import sys
import hashlib
import argparse
import platform
import datetime
import time
import wx
import math
import heapq
# from fileinput import filename
#filename = os.path.abspath(__file__) #os.path.abspath(os.path.dirname(sys.argv[0]))


class FileAndTime(object):

    def __init__(self, file_path):
        self.filepath = file_path
        self.mtime = os.path.getmtime(file_path);

    def __cmp__(self, other):
        return cmp(self.mtime, other.mtime)

    def __lt__(self, other):
        return self.mtime > other.mtime

    def get(self):
        return self.filepath

    
alldirs = set()

def getListOfFiles(dirName):
    if dirName in alldirs:
        return list()
    alldirs.add(dirName)
    # create a list of file and sub directories 
    # names in the given directory 
    listOfFile = os.listdir(dirName)
    allFiles = list()
    # Iterate over all the entries
    for entry in listOfFile:
        # Create full path
        fullPath = os.path.join(dirName, entry)
        # If entry is a directory then get the list of files in this directory 
        if os.path.isdir(fullPath):
            allFiles = allFiles + getListOfFiles(fullPath)
        else:
            allFiles.append(fullPath)
                
    return allFiles

file_times = {}

class MainFrame(wx.Frame):

    def __init__(self):
        wx.Frame.__init__(self, None, -1, 'Find Recent Files (pid {})'.format(os.getpid()),
                          size=(600, 300))
        panel = wx.Panel(self, -1)
                
        self.target_tc = wx.TextCtrl(panel)
        self.target_button = wx.Button(panel, -1, "Target Dir", pos=(50, 20))
        self.Bind(wx.EVT_BUTTON, self.OnChooseTarget, self.target_button)
        
        self.result_tc = wx.TextCtrl(panel, style = wx.TE_MULTILINE)
        
        self.find_button = wx.Button(panel, -1, "Find", pos=(50, 20))
        self.Bind(wx.EVT_BUTTON, self.OnClick, self.find_button)
        self.find_button.SetDefault()
        self.exit_button = wx.Button(panel, -1, "Exit", pos=(50, 20))
        self.Bind(wx.EVT_BUTTON, self.OnExit, self.exit_button)
        
        lbl = wx.StaticText(panel,
                            label="python " + platform.python_version() + " on " + platform.system() + " " + wx.__file__)
        
        bagSizer = wx.GridBagSizer(hgap=5, vgap=5)
        row = 0
        col = 0
        bagSizer.Add(self.target_button, pos=(row, 0),
             flag=wx.EXPAND | wx.ALL,
             border=1)
        bagSizer.Add(self.target_tc, pos=(row, 1), span=(1, 3),
             flag=wx.EXPAND | wx.ALL,
             border=1)
        row += 1

        bagSizer.Add(self.result_tc, pos=(row, 0), span=(1, 3),
             flag=wx.EXPAND | wx.ALL,
             border=1)
        row += 1

        bagSizer.Add(self.find_button, pos=(row, 0),
                     flag=wx.EXPAND | wx.ALL,
                     border=1)
        bagSizer.Add(self.exit_button, pos=(row, 1),
                     flag=wx.ALL | wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_CENTER_HORIZONTAL,
                     border=1)  
        row += 1    
        bagSizer.Add(lbl, pos=(row, 0), span=(1, 3),
                     flag=wx.EXPAND | wx.ALL,
                     border=1)
        bagSizer.AddGrowableCol(2, 0)
        bagSizer.AddGrowableRow(1, 0)
        panel.SetSizerAndFit(bagSizer)

    def OnChooseTarget(self, event):
        with wx.DirDialog(self, "Open target directory",
                   style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST) as dirDialog:

            if dirDialog.ShowModal() != wx.ID_CANCEL:
                self.target_tc.SetValue(dirDialog.GetPath())
                
    def OutputLine(self, line):
        if line.endswith('\n'):
            self.result_tc.AppendText(line)
        else:
            self.result_tc.AppendText(line+'\n')
            
    def OnClick(self, event):
        if not os.path.exists(self.target_tc.GetValue()):
            dlg = wx.MessageDialog(self, "Target directory does not exist - " + self.target_tc.GetValue())
            dlg.ShowModal()            
            return
        
        allfiles = getListOfFiles(self.target_tc.GetValue())
            
        files_heap = []
        ignore_list = ['/.metadata/', '/.git/']
        for onefile in allfiles:
            ignore_flag = False
            for ignore_folder in ignore_list:
                if ignore_folder in onefile:
                    ignore_flag = True
                    break
            if ignore_flag:
                continue
            try:
                heapq.heappush(files_heap, FileAndTime(onefile))
            except FileNotFoundError as ex:
                print(ex, file=sys.stderr)
        for i in range(min(100, len(files_heap))):
            self.OutputLine(heapq.heappop(files_heap).get())
        self.OutputLine("Checked {} files".format(len(allfiles)))
        
    def OnExit(self, event):
        self.Close(True)
        
if __name__ == '__main__':
    print("python " + platform.python_version() + " on " + platform.system())
    parser = argparse.ArgumentParser(description='Build cassini-regress.')
    parser.add_argument("--target", default=[], type=str, nargs='+',
                    help="The directories to search for most recent files")
    parser.add_argument("--ignore", default=['/.metadata/', '/.git/'], type=str, nargs='+',
                    help="The directories to ignore")
    parser.add_argument("--num", default=20, type=int, 
                    help="The number of recent files to display")
    args = parser.parse_args()
    
    print('Current time: ' + datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y"))
    prog_start_time = time.time()
    if len(sys.argv) < 2:
        app = wx.App()
        frame = MainFrame()
        frame.Show()
        app.MainLoop()
        sys.exit(0)
    
    if len(args.target) < 1:
        parser.print_help(sys.stderr)
        sys.exit(1)
        
    files_heap = []
    for folder in args.target:
        allfiles = getListOfFiles(folder)
        print("Check {} files".format(len(allfiles)))
        for onefile in allfiles:
            ignore_flag = False
            for ignore_folder in args.ignore:
                if ignore_folder in onefile:
                    ignore_flag = True
                    break
            if ignore_flag:
                continue
            heapq.heappush(files_heap, FileAndTime(onefile))
    for i in range(min(args.num, len(files_heap))):
        print(heapq.heappop(files_heap).get())
    sys.exit(0)
