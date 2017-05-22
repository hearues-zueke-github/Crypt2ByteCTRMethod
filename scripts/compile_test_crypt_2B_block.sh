#! /bin/bash

gcc -Wall source/utils.c source/test_crypt_2B_block.c -o binaries/test_crypt_2B_block
./binaries/test_crypt_2B_block
