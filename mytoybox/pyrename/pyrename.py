import os, sys, datetime, time
from stat import *

def renametool (source_dir):
	for one_file in os.listdir(source_dir):
		pathname = os.path.join(source_dir, one_file)
		mode = os.stat(pathname)[ST_MODE]
		if S_ISREG(mode):
			last_modify = datetime.datetime.fromtimestamp(os.stat(pathname)[ST_MTIME])	
			# get extension				
			new_file_name = "{0:04}{1:02}{2:02}{3:02}{4:02}{5:02}".format(last_modify.year, last_modify.month, last_modify.day, last_modify.hour, last_modify.minute, last_modify.second)
			ext_pos = one_file.find('.');
			if ext_pos != -1:
				new_file_name = new_file_name+one_file[ext_pos:]
			print (new_file_name)
			new_full_path = os.path.join(source_dir, new_file_name)
			if new_full_path != pathname:
				os.rename(pathname, new_full_path)
				print ('rename ', pathname, ' to ', new_full_path)

if __name__ == "__main__":
	for arg in sys.argv:
		print (arg)
	if len(sys.argv)>1:
		renametool (sys.argv[1])		