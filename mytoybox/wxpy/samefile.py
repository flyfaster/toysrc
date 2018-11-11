#!/usr/bin/env python3
# Python program to find files with the same digest
import os
import sys
import hashlib
import argparse
import platform
import datetime
import time
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
    with open(filename,"rb") as f:
        # Read and update hash string value in blocks of 4M
        for byte_block in iter(lambda: f.read(4096*1024),b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()

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