#! /usr/bin/python2.7

from subprocess import call
import os

rootdir = "/home/haris/test_tree_folder"
rootdir_input = ""
rootdir_output = ""

for root, subdirs, files in os.walk(rootdir):
    print("root = "+str(root))
    print("subdirs = "+str(subdirs))
    print("files = "+str(files))
    pass
    # for f in files:
    #     print("size of "+str(f)+": "+str(os.stat(str(root+"/"+f)).st_size)+" bytes")
    # print("")

call(["../binaries/crypt_2B_ctr_generic", "-c", "key_2B_e_10_r_2.byte", "10", "2"])
call(["../binaries/crypt_2B_ctr_generic", "-c", "key_2B_e_10_r_2.byte", "10", "2"])
