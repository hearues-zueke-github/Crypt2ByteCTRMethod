#ifndef CRYPT_2B_GENERIC_H_INCLUDED
#define CRYPT_2B_GENERIC_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "../include/utils.h"

typedef struct CryptInfo_ {
//    char file_name_in_[128];
//    char file_name_out_[128];
//    char key_file_name_[128];
    uint32 rounds_;
    uint32 block_size_;

} CryptInfo;

void encyrpt_file(char* file_in, char* file_out, uint8** fe_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds);
void decyrpt_file(char* file_in, char* file_out, uint8** fd_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds);

#endif /* CRYPT_2B_GENERIC_H_INCLUDED */
