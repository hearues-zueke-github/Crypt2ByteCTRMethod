#! /usr/bin/python2.7

import os

rootdir = "/home/haris/Documents"

for root, subdirs, files in os.walk(rootdir):
    print("root = "+str(root))
    print("subdirs = "+str(subdirs))
    print("files = "+str(files))
    for f in files:
        print("size of "+str(f)+": "+str(os.stat(str(root+"/"+f)).st_size)+" bytes")
    print("")
