#include "../include/test_print_block.h"
#include "../include/utils.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
  // Test 1 Byte
  init_srand();
  uint8* block;
  int length = 0x55;
  int dim = 1;
  int l[1] = {length};
  create_array_generic((void*)&block, l, dim, sizeof(uint8));
  define_random_1d_block(block, length);
  print_block_1B(block, length);
  print_block_2B(block, length);
  free_array_generic((void*)block, l, dim);

  // Test 2 Bytes
  uint16* block2B;
  int lenght2 = 0x24;
  int dim2 = 1;
  int l2[1] = {lenght2};
  create_array_generic((void*)&block2B, l2, dim2, sizeof(uint16));
  define_random_1d_block((uint8*)block2B, lenght2*2);
  print_block_2B((uint8*)block2B, lenght2*2);
  free_array_generic((void*)block2B, l2, dim2);

  return 0;
}
