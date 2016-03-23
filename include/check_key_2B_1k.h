#ifndef CHECK_KEY_2B_1K_H_INCLUDED
#define CHECK_KEY_2B_1K_H_INCLUDED

#include "crypt_2B_1k.h"

// #define PRINT_BLOCKS false

// Checks only one function block, if every value comes 1 time
int check_f_block(uint8** block);

// Checks, if fp(f(i)) equals to i 
// Otherwise the prime or inverse function would not make sence
int check_f_fp_block(uint8** block_f, uint8** block_fp);

#endif /* CHECK_KEY_2B_1K_H_INCLUDED */
