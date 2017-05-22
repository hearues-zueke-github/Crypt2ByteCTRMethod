#include <stdio.h>
#include <stdlib.h>

#include "../include/utils.h"
#include "../include/endecryptutils.h"
#include "../include/crypt_2B_generic.h"

#define MALLOC_ALL \
  create_array_generic((void*)&fe_block, l_f, dim_f, size_f); \
  create_array_generic((void*)&fd_block, l_f, dim_f, size_f); \
  create_array_generic((void*)&func_key_block, l_f_k, dim_f_k, size_f_k); \
  create_array_generic((void*)&key_block, l_k, dim_k, size_k); \
  create_array_generic((void*)&hash_blocks, l_h, 2, 1); \
  create_array_generic((void*)&hash_blocks_new_calc, l_h, 2, 1);

#define FREE_ALL \
  free_array_generic((void*)fe_block, l_f, dim_f); \
  free_array_generic((void*)fd_block, l_f, dim_f); \
  free_array_generic((void*)func_key_block, l_f_k, dim_f_k); \
  free_array_generic((void*)key_block, l_k, dim_k); \
  free_array_generic((void*)hash_blocks, l_h, 2); \
  free_array_generic((void*)hash_blocks_new_calc, l_h, 2);

void encyrpt_file(char* file_in, char* file_out, uint8** fe_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds) {
  int block_size = 1<<exponent;
  uint8 block[block_size];

  FILE* fin = fopen(file_in, "rb");
  FILE* fout = fopen(file_out, "wb");

  int file_size = fsize(fin);
  int amount_of_blocks = (file_size-1)/block_size+1;

  uint64 temp[1];
  temp[0] = 0;
  fwrite(temp, 1, sizeof(uint64), fout);
  temp[0] = file_size;
  fwrite(temp, 1, sizeof(uint64), fout);

  // First get the block of bytes from file, then apply to encrypt and then save it ot the output file
  int i;
  for (i = 0; i < amount_of_blocks; i++) {
    if (file_size - i*block_size >= block_size)
      fread(block, 1, block_size, fin);
    else {
      memset(block, 0, sizeof(block));
      printf("set all zeros for block, because fin is smaller than block_size!\n");
      fread(block, 1, file_size%block_size, fin);
    }

//    printf("read in block for encryption:\n");
//    print_block_1B(block, block_size);

    int j;
    int k;
    for (j = 0; j < rounds; j++)
      for (k = 0; k < exponent; k++)
        apply_fBlock_2B_with_key_normal(block, block_size, fe_block, func_key_block[key_block[i][j]][k], k);

    fwrite(block, 1, block_size, fout);
  }

  fclose(fin);
  fclose(fout);
}

void decyrpt_file(char* file_in, char* file_out, uint8** fd_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds) {
  int block_size = 1<<exponent;
  uint8 block[block_size];

  FILE* fin = fopen(file_in, "rb");
  FILE* fout = fopen(file_out, "wb");

  int file_size = fsize(fin) - 16;

  uint64 temp[1];
  fread(temp, 1, sizeof(uint64), fin);
  fread(temp, 1, sizeof(uint64), fin);
  int file_size_orig = temp[0];
  printf("File size orig = 0x%X\n", file_size_orig);

  int is_orig_smaller_equal = file_size_orig <= file_size;
  printf("Original File is %s of the read in file!\n", (is_orig_smaller_equal ? "smaller/equal" : "bigger"));
  int is_file_partioned = !(file_size % block_size);
  printf("Read in file %s\n", (is_file_partioned ? "partioned, therefore it is possible to be OK!" : "is not partion able!!! Something is wrong?!?!"));

  int amount_of_blocks = file_size / block_size;
  printf("amount_of_blocks = %d\n", amount_of_blocks);

  // First get the block of bytes from file, then apply to encrypt and then save it ot the output file
  int i;
  for (i = 0; i < amount_of_blocks; i++) {
    fread(block, 1, block_size, fin);

    printf("read in block for DEcryption:\n");
    print_block_1B(block, block_size);

    int j;
    int k;
    for (j = rounds; j > 0; j--)
      for (k = exponent; k > 0; k--)
        apply_fBlock_2B_with_key_normal(block, block_size, fd_block, func_key_block[key_block[i][j-1]][k-1], k-1);

    if (file_size_orig - i*block_size >= block_size)
      fwrite(block, 1, block_size, fout);
    else
      fwrite(block, 1, file_size_orig%block_size, fout);
  }

  fclose(fin);
  fclose(fout);

//  fin = fopen(file_in, "rb");
//  fout = fopen(file_out, "wb");

//  fclose(fin);
//  fclose(fout);
}

int main(int argc, char* argv[]) {
  // First create the fe and fd blocks
  int exponent = 8;
  int rounds = 2;

  uint64 key_iterations = 1<<(exponent-1);

  uint8**  hash_blocks;
  uint8**  hash_blocks_new_calc;
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

  MALLOC_ALL;

  load_key_2B_generic("../key_files/key_2B_e_8_r_2.byte", fe_block, hash_blocks, fd_block, func_key_block, key_block, &exponent, &rounds);
  printf("successfully loaded into blocks!!\n");

//  printf("0x%X 0x%X\n", fe_block[0][0x0000], fe_block[0][0x0001]);
//  int combine = ((fe_block[0][0x0000]<<8)+fe_block[0][0x0001])*2;
//  printf("0x%X 0x%X\n", fd_block[0][combine+0], fd_block[0][combine+1]);

//  FREE_ALL;
//  return 0;

  calc_hash_of_2D_block(hash_blocks_new_calc[0], 32, fe_block, l_f[1], l_f[0]);
  calc_hash_of_2D_block(hash_blocks_new_calc[1], 32, fd_block, l_f[1], l_f[0]);
  calc_hash_of_3D_block(hash_blocks_new_calc[2], 32, func_key_block, l_f_k[2], l_f_k[1], l_f_k[0]);
  calc_hash_of_2D_block(hash_blocks_new_calc[3], 32, key_block, l_k[1], l_k[0]);

  // Create function for checking hash value

//  printf("printing hash block of fe_block:\n");
//  print_block_1B(hash_blocks[0], 32);
//  printf("printing hash block of fd_block:\n");
//  print_block_1B(hash_blocks[1], 32);
//  printf("printing hash block of func_key_block:\n");
//  print_block_1B(hash_blocks[2], 32);
//  printf("printing hash block of key_block:\n");
//  print_block_1B(hash_blocks[3], 32);

//  printf("\nprinting hash block of fe_block (new calculated):\n");
//  print_block_1B(hash_blocks_new_calc[0], 32);
//  printf("printing hash block of fd_block (new calculated):\n");
//  print_block_1B(hash_blocks_new_calc[1], 32);
//  printf("printing hash block of func_key_block (new calculated):\n");
//  print_block_1B(hash_blocks_new_calc[2], 32);
//  printf("printing hash block of key_block (new calculated):\n");
//  print_block_1B(hash_blocks_new_calc[3], 32);

  printf("hash of fe_block       : %s\n", (check_hash_block(hash_blocks[0], hash_blocks_new_calc[0], 32) ? "\x1B[32mis OK\x1B[0m" : "\x1B[31mis NOT OK!!!\x1B[0m"));
  printf("hash of fd_block       : %s\n", (check_hash_block(hash_blocks[1], hash_blocks_new_calc[1], 32) ? "\x1B[32mis OK\x1B[0m" : "\x1B[31mis NOT OK!!!\x1B[0m"));
  printf("hash of func_key_block : %s\n", (check_hash_block(hash_blocks[2], hash_blocks_new_calc[2], 32) ? "\x1B[32mis OK\x1B[0m" : "\x1B[31mis NOT OK!!!\x1B[0m"));
  printf("hash of key_block      : %s\n", (check_hash_block(hash_blocks[3], hash_blocks_new_calc[3], 32) ? "\x1B[32mis OK\x1B[0m" : "\x1B[31mis NOT OK!!!\x1B[0m"));

//  print_block_2B(fe_block[0], 0x20000);
//  print_block_2B(fd_block[0], 0x20000);

  printf("fe[0][0] = 0x%2X 0x%2X\n", fe_block[0][0], fe_block[0][1]);
  printf("fd[0][0x7faa] = 0x%2X 0x%2X\n", fd_block[0][0x7faa*2+0], fd_block[0][0x7faa*2+1]);

  printf("Now test encryption\n");
  encyrpt_file("../files/test_for_32_bytes.byte", "../files/test_for_32_bytes_encrypted.byte", fe_block, func_key_block, key_block, exponent, rounds);
  printf("Now test DEcryption\n");
  decyrpt_file("../files/test_for_32_bytes_encrypted.byte", "../files/test_for_32_bytes_DEcrypted.byte", fd_block, func_key_block, key_block, exponent, rounds);

  FREE_ALL;
  printf("Finish all free!\n");

  return 0;
}
