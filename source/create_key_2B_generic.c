#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/utils.h"
#include "../include/create_key_2B_generic.h"

int main(int argc, char* argv[]) {
  printf("Amount of cores: %d\n", (int)sysconf(_SC_NPROCESSORS_ONLN));

  // We will only create here a key for 2 byte en/de cryption!!!
  uint64 exponent = 8;
  uint64 rounds = 2;

  uint64 key_iterations = 1<<(exponent-1);

  uint8**  hash_blocks;
  uint8**  fe_block;
  uint8**  fd_block;
  uint8*** func_key_block;
  uint8**  key_block;

  int m_f = 0x100;
  int n_f = 0x10000;
  int l_f[] = {n_f*2, m_f};
  int dim_f = 2;
  int size_f = sizeof(uint8);

  int l_f_k[] = {key_iterations, exponent, 0x100};
  int dim_f_k = 3;
  int size_f_k = sizeof(uint8);

  int l_k[] = {rounds, 1<<(32-exponent)};
  int dim_k = 2;
  int size_k = sizeof(uint8);

  int l_h[] = {32, 4};

//  uint64 size_fe_block = l_f[0] * l_f[1];
//  uint64 size_fd_block = l_f[0] * l_f[1];
//  uint64 size_func_key_block = l_f_k[0] * l_f_k[1] * l_f_k[2];
//  uint64 size_key_block =  l_k[0] * l_k[1];

//  printf("size of fe block       : 0x%8x\n", size_fe_block);
//  printf("size of fd block       : 0x%8x\n", size_fd_block);
//  printf("size of func key block : 0x%8x\n", size_func_key_block);
//  printf("size of key block      : 0x%8x\n", size_key_block);

  printf("Create arrays for cryption.\n");
  create_array_generic((void*)&fe_block, l_f, dim_f, size_f);
  create_array_generic((void*)&fd_block, l_f, dim_f, size_f);
  create_array_generic((void*)&func_key_block, l_f_k, dim_f_k, size_f_k);
  create_array_generic((void*)&key_block, l_k, dim_k, size_k);
  create_array_generic((void*)&hash_blocks, l_h, 2, 1);

  printf("Define fe_block\n");
  define_f_Block_Generic(fe_block, m_f, n_f);
  printf("Define fd_block\n");
  define_fi_Block_Generic(fe_block, fd_block, m_f, n_f);
  printf("Define func_key_block\n");
  define_random_3d_block(func_key_block, l_f_k[2], l_f_k[1], l_f_k[0]);
  printf("Define key_block\n");
  define_random_2d_block(key_block, l_k[1], l_k[0]);

  printf("after:\n");
  print_block_2B(fe_block[0], 0x200);
  print_block_2B(fd_block[0], 0x200);
//  printf("fd_block[0][0] = 0x%2X\n", fd_block[0][0]);
//  printf("fd_block[0][1] = 0x%2X\n", fd_block[0][1]);

  int pos = 0x0000;
  printf("encryption 0x%X 0x%X\n", fe_block[0][pos*2+0], fe_block[0][pos*2+1]);
  int combine = ((fe_block[0][pos*2]<<8)+fe_block[0][pos*2+1])*2;
  printf("combine = 0x%4X\n", combine/2);
  printf("decryption 0x%X 0x%X\n", fd_block[0][combine+0], fd_block[0][combine+1]);

  pos = 0x0001;
  printf("encryption 0x%X 0x%X\n", fe_block[0][pos*2+0], fe_block[0][pos*2+1]);
  combine = ((fe_block[0][pos*2]<<8)+fe_block[0][pos*2+1])*2;
  printf("combine = 0x%4X\n", combine/2);
  printf("decryption 0x%X 0x%X\n", fd_block[0][combine+0], fd_block[0][combine+1]);

  pos = 0x0002;
  printf("encryption 0x%X 0x%X\n", fe_block[0][pos*2+0], fe_block[0][pos*2+1]);
  combine = ((fe_block[0][pos*2]<<8)+fe_block[0][pos*2+1])*2;
  printf("combine = 0x%4X\n", combine/2);
  printf("decryption 0x%X 0x%X\n", fd_block[0][combine+0], fd_block[0][combine+1]);

//  printf("Press Enter!\n");
//  char c = getchar();

//  print_block_2B(fe_block[0], 0x20000);
//  print_block_2B(fd_block[0], 0x20000);
//  print_block_1B(func_key_block[0][0], l_f_k[0]);
//  print_block_1B(key_block[0], l_k[0]);

  printf("Calc hash of blocks.\n");
  calc_hash_of_2D_block(hash_blocks[0], 32, fe_block, l_f[1], l_f[0]);
  calc_hash_of_2D_block(hash_blocks[1], 32, fd_block, l_f[1], l_f[0]);
  calc_hash_of_3D_block(hash_blocks[2], 32, func_key_block, l_f_k[2], l_f_k[1], l_f_k[0]);
  calc_hash_of_2D_block(hash_blocks[3], 32, key_block, l_k[1], l_k[0]);

//  printf("printing hash block of fe_block:\n");
//  print_block_1B(hash_blocks[0], 32);
//  printf("printing hash block of fd_block:\n");
//  print_block_1B(hash_blocks[1], 32);
//  printf("printing hash block of func_key_block:\n");
//  print_block_1B(hash_blocks[2], 32);
//  printf("printing hash block of key_block:\n");
//  print_block_1B(hash_blocks[3], 32);

  printf("Saving key to file.\n");
  save_key_2B_generic("../key_files/key_2B_e_8_r_2.byte", fe_block, hash_blocks, fd_block, func_key_block, key_block, exponent, rounds);

  printf("Free fe_block\n");
  free_array_generic((void*)fe_block, l_f, dim_f);
  printf("Free fd_block\n");
  free_array_generic((void*)fd_block, l_f, dim_f);
  printf("Free func_key_block\n");
  free_array_generic((void*)func_key_block, l_f_k, dim_f_k);
  printf("Free key_block\n");
  free_array_generic((void*)key_block, l_k, dim_k);
  printf("Free hash_blocks\n");
  free_array_generic((void*)hash_blocks, l_h, 2);
  printf("Finish all free!\n");

  return 0;
}
