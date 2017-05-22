#! /usr/bin/python3.5

import os
import sys

from subprocess import call

if __name__ == "__main__":
    argv = sys.argv
    if len(argv) < 5:
        print("usage: (python2|python3) <key_file> (encryption|decryption) <folder_from> <folder_to>")
        sys.exit(1)

    key_file_name = argv[1]
    modus = argv[2]
    root_from = argv[3]
    root_to = argv[4]

    if modus != "encryption" and modus != "decryption":
        print("modus must be either 'encryption' or 'decryption'!\n{} is not possible!".format(modus))
        sys.exit(2)

    if not os.path.exists(root_from):
        print("path '{}' does not exist!".format(root_from))
        sys.exit(3)

    root_to_paths = []
    root_to_paths_files = []

    for path, _, files in os.walk(root_from):
        root_to_paths.append(path)
        root_to_paths_files.append((path, files))

    root_to_paths_files = sorted(root_to_paths_files, key=lambda x: x[0])

    # delete a path, which is contained in other root_to_paths
    root_to_paths = sorted(root_to_paths)
    found_paths = []
    for path1, path2 in zip(root_to_paths[:-1], root_to_paths[1:]):
        if not path1 in path2:
            found_paths.append(path1)
    found_paths.append(root_to_paths[-1])


    print("len(root_to_paths): {}".format(len(root_to_paths)))
    print("len(found_paths): {}".format(len(found_paths)))

    root_from_paths = [path.replace(root_from, root_to) for path in found_paths]
    print("All possible accessed directories will be created first:")
    for path in root_from_paths:
        if not os.path.exists(path):
            os.makedirs(path)
        print("  {}".format(path))

    for path, files in root_to_paths_files:
        for file in files:
            call(["./ctr_encryption", key_file_name, modus,
                os.path.join(path, file),
                os.path.join(path.replace(root_from, root_to), file)])
