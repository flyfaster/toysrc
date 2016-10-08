from __future__ import print_function
import subprocess 
import sys
import argparse
import time
import datetime
#pid = os.spawnlp(os.P_NOWAIT, "/bin/mycmd", "mycmd", "myarg")
# check /proc/sys/kernel/pid_max
# ulimit -u

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Accepted arguments')
    parser.add_argument('-n','--num', help='number of processes to start', required=True)
    args = vars(parser.parse_args())
    start_time = datetime.datetime.now()
    allpid=[]
    try:
        for i in range(0, int(args['num'])):
            time.sleep(0.01)
            child = subprocess.Popen(["/home/onega/github/toysrc/mytoybox/boost/asiotimeout/client", "127.0.0.1:1234", "./client"])
            pid = child.pid
            print(pid, sep=' ', end=" ")
            allpid.append(child)
            if i%16 == 15 :
                print("")
    except OSError as ose:
         print ("OS error({0}): {1}".format(ose.errno, ose.strerror))
    if(len(allpid)%16>0):
        print("")    
    end_time = datetime.datetime.now()
    duration = end_time - start_time
    print("It took {} seconds to start {} processes".format(duration.total_seconds(), len(allpid)))
    for kid in allpid:
        kid.wait()
    end2 = datetime.datetime.now()
    duration = end2 - end_time 
    print("All kids exit after {} seconds".format(duration.total_seconds()))
    sys.exit(0)