#include "../include/endecryptutils.h"

void apply_f_block_1B_on_block(uint8* block, int size, uint8* f_block) {
  int i;
  for (i = 0; i < size; i++)
    block[i] = f_block[block[i]];
}

void apply_f_block_2B_on_block_normal(uint8* block, int size, uint8* f_block) {
  int i;
  int temp;
  for (i = 0; i < size/2; i++) {
    temp = ((block[2*i+0]<<8)+block[2*i+1])*2;
    block[2*i+0] = f_block[temp+0];
    block[2*i+1] = f_block[temp+1];
  }
}

void apply_f_block_2B_on_block_shift(uint8* block, int size, uint8* f_block) {
  int i;
  int temp;
  for (i = 0; i < size/2-1; i++) {
    temp = ((block[2*i+1]<<8)+block[2*i+2])*2;
    block[2*i+1] = f_block[temp+0];
    block[2*i+2] = f_block[temp+1];
  }

  temp = ((block[size-1]<<8)+block[0])*2;
  block[size-1] = f_block[temp+0];
  block[0]      = f_block[temp+1];
}

void apply_fBlock_2B_with_key_normal(uint8* block, int size, uint8** f_block, uint8* f_k_block, int spacing) {
  int i;
  int j;
  int k1;
  int k2;
  int temp;
  int shift = 1<<spacing;
  for (j = 0; j < size/(shift*2); j++)
    for (i = 0; i < shift; i++) {
      k1 = i + j*shift*2;
      k2 = i + shift + j*shift*2;
      temp = ((block[k1]<<8)+block[k2])*2;
//      printf("k1=%d   k2=%d   temp=0x%x   block[k1]=0x%x   block[k2]=0x%x   ", k1, k2, temp, block[k1], block[k2]);
      block[k1] = f_block[f_k_block[j*shift+i]][temp+0];
      block[k2] = f_block[f_k_block[j*shift+i]][temp+1];
//      printf("after block[k1] = 0x%X   block[k2] = 0x%X\n", block[k1], block[k2]);
    }
}
