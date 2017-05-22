#ifndef CREATE_KEY_2B_1K_H_INCLUDED
#define CREATE_KEY_2B_1K_H_INCLUDED

#include "crypt_2B_1k.h"

// Def the function block
void define_f_Block(uint8** block);

// Def the function prime block
void define_fp_Block(uint8** block_f, uint8** block_fp);

// Def the orientation Block
void define_o_Block(uint8*** block);

// Def the crossover Block
void define_c_Block(uint8** block);

#endif /* CREATE_KEY_2B_1K_H_INCLUDED */
