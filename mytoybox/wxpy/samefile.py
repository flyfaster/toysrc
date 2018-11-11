#!/usr/bin/env python3
# Python program to find files with the same digest
import os
import sys
import hashlib
import argparse
import platform
import datetime
import time
import wx
from fileinput import filename
#filename = os.path.abspath(__file__) #os.path.abspath(os.path.dirname(sys.argv[0]))

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


def get_file_checksum(filename):
    sha256_hash = hashlib.sha256()
    try:
        with open(filename, "rb") as f:
            # Read and update hash string value in blocks of 4M
            for byte_block in iter(lambda: f.read(4096 * 1024), b""):
                sha256_hash.update(byte_block)
    except FileNotFoundError as ex:
        print(ex, file=sys.stderr)
        pass
    except OSError as ex:
        print(ex, file=sys.stderr)
        pass
    return sha256_hash.hexdigest()


class MainFrame(wx.Frame):

    def __init__(self):
        wx.Frame.__init__(self, None, -1, 'Find Same Files (pid {})'.format(os.getpid()),
                          size=(600, 300))
        panel = wx.Panel(self, -1)
        self.base_tc = wx.TextCtrl(panel)
        self.base_button = wx.Button(panel, -1, "Base File", pos=(50, 20))
        self.Bind(wx.EVT_BUTTON, self.OnChooseBase, self.base_button)
        
        self.target_tc = wx.TextCtrl(panel)
        self.target_button = wx.Button(panel, -1, "Target Dir", pos=(50, 20))
        self.Bind(wx.EVT_BUTTON, self.OnChooseTarget, self.target_button)
        
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
        bagSizer.Add(self.base_button, pos=(row, 0),
             flag=wx.EXPAND | wx.ALL,
             border=1)
        bagSizer.Add(self.base_tc, pos=(row, 1), span=(1, 3),
             flag=wx.EXPAND | wx.ALL,
             border=1)
        row += 1
        bagSizer.Add(self.target_button, pos=(row, 0),
             flag=wx.EXPAND | wx.ALL,
             border=1)
        bagSizer.Add(self.target_tc, pos=(row, 1), span=(1, 3),
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
        panel.SetSizerAndFit(bagSizer)

    def OnChooseBase(self, event):
        with wx.FileDialog(self, "Open base file", wildcard="base files (*.*)|*",
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
        with wx.DirDialog(self, "Open target directory",
                   style=wx.FD_OPEN | wx.FD_FILE_MUST_EXIST) as dirDialog:

            if dirDialog.ShowModal() != wx.ID_CANCEL:
                self.target_tc.SetValue(dirDialog.GetPath())
                    
    def OnClick(self, event):
        if not os.path.exists(self.base_tc.GetValue()):
            dlg = wx.MessageDialog(self, "Base file does not exist - " + self.base_tc.GetValue())
            dlg.ShowModal()
            return
        if not os.path.exists(self.target_tc.GetValue()):
            dlg = wx.MessageDialog(self, "Target directory does not exist - " + self.target_tc.GetValue())
            dlg.ShowModal()            
            return
        base_checksum = get_file_checksum(self.base_tc.GetValue())
        allfiles = getListOfFiles(self.target_tc.GetValue())
        print("Check {} files".format(len(allfiles)))
        for onefile in allfiles:
            if get_file_checksum(onefile) == base_checksum:
                print(onefile)
              
    def OnExit(self, event):
        self.Close(True)

        
if __name__ == '__main__':
    print("python " + platform.python_version() + " on " + platform.system())
    parser = argparse.ArgumentParser(description='Build cassini-regress.')
    parser.add_argument("--target", default=[], type=str, nargs='+',
                    help="The directories to search for files with same checksum")
    parser.add_argument("-b", "--base", type=str,
                    help="The base file path.")
    args = parser.parse_args()
    
    print('Current time: ' + datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y"))
    prog_start_time = time.time()
    if len(sys.argv) < 2:
        app = wx.App()
        frame = MainFrame()
        frame.Show()
        app.MainLoop()
        sys.exit(0)
    
    if args.base is None or len(args.target) < 1 or not os.path.exists(args.base):
        parser.print_help(sys.stderr)
        sys.exit(1)
        
    base_checksum = get_file_checksum(args.base)
    for folder in args.target:
        allfiles = getListOfFiles(folder)
        print("Check {} files".format(len(allfiles)))
        for onefile in allfiles:
            if get_file_checksum(onefile) == base_checksum:
                print(onefile)
    sys.exit(0)
