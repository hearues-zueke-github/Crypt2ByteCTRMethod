#ifndef ENDECRYPTUTILS_H_INCLUDED
#define ENDECRYPTUTILS_H_INCLUDED

#include "utils.h"

/**
 * \param  block    Block of 1B type
 * \param  size     The length of block
 * \param  f_block  For 1B of block has exaclty a length of 256, every value comes ones!
 */
void apply_f_block_1B_on_block(uint8* block, int size, uint8* f_block);

/**
 * \param  block    Block of 2B type
 * \param  size     The length of block
 * \param  f_block  For 2B of block has exaclty a length of 65536, every value comes ones!
 */
void apply_f_block_2B_on_block_normal(uint8* block, int size, uint8* f_block);

/**
 * \param  block    Block of 2B type
 * \param  size     The length of block
 * \param  f_block  For 2B of block has exaclty a length of 65536, every value comes ones!
 */
void apply_f_block_2B_on_block_shift(uint8* block, int size, uint8* f_block);

void apply_fBlock_2B_with_key_normal(uint8* block, int size, uint8** f_block, uint8* f_k_block, int spacing);

#endif /* ENDECRYPTUTILS_H_INCLUDED */