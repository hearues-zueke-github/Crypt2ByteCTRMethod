#ifndef CRYPT_2B_CTR_GENERIC_H_INCLUDED
#define CRYPT_2B_CTR_GENERIC_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "../include/utils.h"

void create_2B_ctr_key(char* filename_key, uint64 exponent, uint64 rounds);

void encyrpt_file(char* file_in, char* file_out, uint8* nonce_block, uint8** f_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds);

void decyrpt_file(char* file_in, char* file_out, uint8* nonce_block, uint8** f_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds);

void crypt_program(int modus, char* filename_key, char* filename_input, char* filename_output);

int isnumber(const char * s);

#endif /* CRYPT_2B_CTR_GENERIC_H_INCLUDED */
