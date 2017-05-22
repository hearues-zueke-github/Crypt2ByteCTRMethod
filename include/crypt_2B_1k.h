#ifndef CRYPT_2B_1k_H
#define CRYPT_2B_1k_H

#include "utils.h"

#define BYTES 2
#define BYTES_SHIFT 1
#define BLOCK 1024
// f-Block
#define BLOCK_F_M 256
#define BLOCK_F_N 256 * 256
// o-Block
#define BLOCK_O_B 256
#define BLOCK_O_I 4
// c-Block
#define BLOCK_C_N 1024*1024*4
#define BLOCK_C_I 4

#define PRINT_BLOCKS false

#define MAX_MEM (((BLOCK_F_N <= 0x100 ? BLOCK_F_N : BLOCK_F_N * 2) * BLOCK_F_M * 2) + \
                 ((BLOCK_F_N <= 0x100 ? BLOCK : BLOCK / 2) * BLOCK_O_I * 2 * BLOCK_O_B) + \
                  (BLOCK_C_I * BLOCK_C_N))

#define CREATE_ARRAY \
    create_array_generic((void*)&block_f, l_f, 2, sizeof(uint8));  \
    create_array_generic((void*)&block_fp, l_f, 2, sizeof(uint8)); \
    create_array_generic((void*)&block_o, l_o, 3, sizeof(uint8));  \
    create_array_generic((void*)&block_c, l_c, 2, sizeof(uint8));
#define MALLOC_FAIL ((block_f == 0) || (block_fp == 0) || (block_o == 0) || (block_c == 0))
#define FREE_MALLOC \
    if (block_f != 0)  { free_array_generic((void*)block_f, l_f, 2); }  \
    if (block_fp != 0) { free_array_generic((void*)block_fp, l_f, 2); } \
    if (block_o != 0)  { free_array_generic((void*)block_o, l_o, 3); }  \
    if (block_c != 0)  { free_array_generic((void*)block_c, l_c, 2); }

extern int l_f[];
extern int l_o[];
extern int l_c[];

// Define all blocks at once
// First read the whole keyfile in, then get the seperate blocks
void get_foc_Block(uint8* whole_block, uint8** f_block, uint8** fp_block, uint8*** o_block, uint8** c_block);

#endif /* CRYPT_2B_1k_H */
