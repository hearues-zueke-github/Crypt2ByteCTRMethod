#include "../include/test_crypt_2B_block.h"
#include "../include/utils.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
  init_srand();
  uint8* keyblock;
  int keyblocksize = 0x20;
  int dim = 1;
  int l[1] = {keyblocksize};
  create_array_generic((void*)&keyblock, l, dim, sizeof(uint8));
//  print_block_1B(keyblock, keyblocksize);
//  ((uint64*)keyblock)[3] = 0x123456789ABCDEF0;
//  print_block_1B(keyblock, keyblocksize);
//  uint64 numbers = 0x123456789ABCDEF0;
//  int i;
//  for (i = 0; i < 0x8; i++)
//    printf("numbers[0x%X] = 0x%X\n", i, ((uint8*)&numbers)[i]);
  int i;
  for (i = 0; i < 10; i++) {
    free_array_generic((void*)keyblock, l, dim);
    create_array_generic((void*)&keyblock, l, dim, sizeof(uint8));
//    define_zero_1d_block(keyblock, keyblocksize);
    print_block_1B(keyblock, keyblocksize);
    define_random_1d_block(keyblock, keyblocksize);
  }
  free_array_generic((void*)keyblock, l, dim);
  return 0;
}
