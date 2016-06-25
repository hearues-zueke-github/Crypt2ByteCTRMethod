#! /usr/bin/python3

import os

without_extension = lambda x: ".".join(x.split(".")[:-1])

compiler = "gcc "
flags = "-Wall "
filess = [["create_key_2B_generic.c", ["utils.c"]],
          ["crypt_2B_generic.c", ["utils.c", "endecryptutils.c"]]]

os.chdir("../source")

for files in filess:
    # print("file to compile: {}, without extension: {}".format(files[0], without_extension(files[0])))
    # for i, file in enumerate(files[1]):
    #     print("also needed file: nr. {}, file: {}".format(i, file))

    whole_command = compiler + flags + files[0] + " "
    for file in files[1]:
        whole_command += file+" "

    whole_command += "-o "+without_extension(files[0])
    print("{}".format(whole_command))
    os.system(whole_command)
