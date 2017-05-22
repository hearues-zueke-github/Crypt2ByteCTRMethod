#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "../include/utils.h"
#include "../include/endecryptutils.h"
#include "../include/crypt_2B_ctr_generic.h"

#define MALLOC_ALL \
  create_array_generic((void*)&nonce_block, l_n, 1, 1); \
  create_array_generic((void*)&f_block, l_f, dim_f, size_f); \
  create_array_generic((void*)&func_key_block, l_f_k, dim_f_k, size_f_k); \
  create_array_generic((void*)&key_block, l_k, dim_k, size_k); \
  create_array_generic((void*)&hash_blocks, l_h, 2, 1); \
  create_array_generic((void*)&hash_blocks_new_calc, l_h, 2, 1);

#define FREE_ALL \
  free_array_generic((void*)nonce_block, l_n, 1); \
  free_array_generic((void*)f_block, l_f, dim_f); \
  free_array_generic((void*)func_key_block, l_f_k, dim_f_k); \
  free_array_generic((void*)key_block, l_k, dim_k); \
  free_array_generic((void*)hash_blocks, l_h, 2); \
  free_array_generic((void*)hash_blocks_new_calc, l_h, 2);

void create_2B_ctr_key(char* filename_key, uint64 exponent, uint64 rounds) {
  printf("Amount of cores: %d\n", (int)sysconf(_SC_NPROCESSORS_ONLN));

  uint64 block_size = 1<<exponent;
  uint64 key_iterations = 1<<(exponent-1);

  uint8**  hash_blocks;
  uint8*   nonce_block;
  uint8**  f_block;
  uint8*** func_key_block;
  uint8**  key_block;

  int l_h[] = {32, 4};

  int l_n[] = {block_size};

  int l_f[] = {0x20000, 0x100};
  int dim_f = 2;
  int size_f = sizeof(uint8);

  int l_f_k[] = {key_iterations, exponent, 0x100};
  int dim_f_k = 3;
  int size_f_k = sizeof(uint8);

  int l_k[] = {rounds, 1<<(32-exponent)};
  int dim_k = 2;
  int size_k = sizeof(uint8);

  printf("Create arrays for cryption.\n");
  create_array_generic((void*)&hash_blocks, l_h, 2, 1);
  create_array_generic((void*)&nonce_block, l_n, 1, 1);
  create_array_generic((void*)&f_block, l_f, dim_f, size_f);
  create_array_generic((void*)&func_key_block, l_f_k, dim_f_k, size_f_k);
  create_array_generic((void*)&key_block, l_k, dim_k, size_k);

  printf("Define nonce_block\n");
  define_random_1d_block(nonce_block, l_n[0]);
  printf("Define f_block\n");
  define_f_Block_Generic(f_block, l_f[1], l_f[0]/2);
  printf("Define func_key_block\n");
  define_random_3d_block(func_key_block, l_f_k[2], l_f_k[1], l_f_k[0]);
  printf("Define key_block\n");
  define_random_2d_block(key_block, l_k[1], l_k[0]);

  printf("Calc hash of blocks.\n");
  calc_hash_of_1D_block(hash_blocks[0], 32, nonce_block, l_n[0]);
  calc_hash_of_2D_block(hash_blocks[1], 32, f_block, l_f[1], l_f[0]);
  calc_hash_of_3D_block(hash_blocks[2], 32, func_key_block, l_f_k[2], l_f_k[1], l_f_k[0]);
  calc_hash_of_2D_block(hash_blocks[3], 32, key_block, l_k[1], l_k[0]);

  printf("Hash of nonce_block:\n");
  print_block_1B(hash_blocks[0], l_h[0]);
  printf("\nHash of f_block:\n");
  print_block_1B(hash_blocks[1], l_h[0]);
  printf("\nHash of func_key_block:\n");
  print_block_1B(hash_blocks[2], l_h[0]);
  printf("\nHash of key_block:\n");
  print_block_1B(hash_blocks[3], l_h[0]);

  // char filename[] = "../key_files/key_2B_ctr_e_8_r_2.byte";
  printf("Saving key to file %s.\n", filename_key);
  save_key_2B_ctr_generic(filename_key, hash_blocks, nonce_block, f_block, func_key_block, key_block, exponent, rounds);

  printf("Free hash_blocks\n");
  free_array_generic((void*)hash_blocks, l_h, 2);
  printf("Free nonce_block\n");
  free_array_generic((void*)nonce_block, l_n, 1);
  printf("Free f_block\n");
  free_array_generic((void*)f_block, l_f, dim_f);
  printf("Free func_key_block\n");
  free_array_generic((void*)func_key_block, l_f_k, dim_f_k);
  printf("Free key_block\n");
  free_array_generic((void*)key_block, l_k, dim_k);
  printf("Finish all free!\n");
}

void encyrpt_file(char* file_in, char* file_out, uint8* nonce_block, uint8** f_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds) {
  int block_size = 1<<exponent;
  uint8 block[block_size];
  uint8 nonce_block_temp[block_size];

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
      // printf("set all zeros for block, because fin is smaller than block_size!\n");
      fread(block, 1, file_size%block_size, fin);
    }

    memcpy(nonce_block_temp, nonce_block, block_size);
    // if (i == 0) {
    //   printf("nonce_block and nonce_block_temp before everything\n");
    //   print_block_1B(nonce_block, block_size);
    //   print_block_1B(nonce_block_temp, block_size);
    // }
    uint64 val = (uint64)i;
    memcpy(nonce_block_temp, (uint8*)&val, 8);
    // if (i == 0) {
    //   printf("nonce_block_temp before encryption with value i\n");
    //   print_block_1B(nonce_block_temp, block_size);
    // }

    int j;
    int k;
    for (j = 0; j < rounds; j++)
      for (k = 0; k < exponent; k++)
        apply_fBlock_2B_with_key_normal(nonce_block_temp, block_size, f_block, func_key_block[key_block[i][j]][k], k);

    // if (i == 0) {
    //   printf("nonce_block_temp after encryption\n");
    //   print_block_1B(nonce_block_temp, block_size);
    // }
    xor_two_blocks(block, nonce_block_temp, block_size);

    fwrite(block, 1, block_size, fout);
  }

  fclose(fin);
  fclose(fout);
}

void decyrpt_file(char* file_in, char* file_out, uint8* nonce_block, uint8** f_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds) {
  int block_size = 1<<exponent;
  uint8 block[block_size];
  uint8 nonce_block_temp[block_size];

  FILE* fin = fopen(file_in, "rb");
  FILE* fout = fopen(file_out, "wb");

  int file_size = fsize(fin) - 16;

  uint64 temp[1];
  fread(temp, 1, sizeof(uint64), fin);
  fread(temp, 1, sizeof(uint64), fin);
  int file_size_orig = temp[0];
  // printf("File size orig = 0x%X\n", file_size_orig);

  // int is_orig_smaller_equal = file_size_orig <= file_size;
  // printf("Original File is %s of the read in file!\n", (is_orig_smaller_equal ? "smaller/equal" : "bigger"));
  // int is_file_partioned = !(file_size % block_size);
  // printf("Read in file %s\n", (is_file_partioned ? "partioned, therefore it is possible to be OK!" : "is not partion able!!! Something is wrong?!?!"));

  int amount_of_blocks = file_size / block_size;
  // printf("amount_of_blocks = %d\n", amount_of_blocks);

  // First get the block of bytes from file, then apply to encrypt and then save it ot the output file
  int i;
  for (i = 0; i < amount_of_blocks; i++) {
    fread(block, 1, block_size, fin);

    // printf("read in block for DEcryption:\n");
    // print_block_1B(block, block_size);

    memcpy(nonce_block_temp, nonce_block, block_size);
    uint64 val = (uint64)i;
    memcpy(nonce_block_temp, (uint8*)&val, 8);

    int j;
    int k;
    for (j = 0; j < rounds; j++)
      for (k = 0; k < exponent; k++)
        apply_fBlock_2B_with_key_normal(nonce_block_temp, block_size, f_block, func_key_block[key_block[i][j]][k], k);

    xor_two_blocks(block, nonce_block_temp, block_size);

    if (file_size_orig - i*block_size >= block_size)
      fwrite(block, 1, block_size, fout);
    else
      fwrite(block, 1, file_size_orig%block_size, fout);
  }

  fclose(fin);
  fclose(fout);
}

void crypt_program(int modus, char* filename_key, char* filename_input, char* filename_output) {
  int exponent = 9;
  int rounds = 2;
  
  load_exp_round(filename_key, &exponent, &rounds);

  uint64 key_iterations = 1<<(exponent-1);

  uint8**  hash_blocks;
  uint8**  hash_blocks_new_calc;
  uint8*   nonce_block;
  uint8**  f_block;
  uint8*** func_key_block;
  uint8**  key_block;

  int l_h[] = {32, 4};

  int l_n[] = {1<<exponent};

  int l_f[] = {0x20000, 0x100};
  int dim_f = 2;
  int size_f = sizeof(uint8);

  int l_f_k[] = {key_iterations, exponent, 0x100};
  int dim_f_k = 3;
  int size_f_k = sizeof(uint8);

  int l_k[] = {rounds, 1<<(32-exponent)};
  int dim_k = 2;
  int size_k = sizeof(uint8);

  MALLOC_ALL;

  printf("No segmentation fault yet!!!\n");
  load_key_2B_ctr_generic(filename_key, hash_blocks, nonce_block, f_block, func_key_block, key_block, exponent, rounds);
  printf("successfully loaded into blocks!!\n");
  printf("exponent = %d\n", exponent);
  printf("rounds   = %d\n", rounds);

  calc_hash_of_1D_block(hash_blocks_new_calc[0], 32, nonce_block, l_n[0]);
  calc_hash_of_2D_block(hash_blocks_new_calc[1], 32, f_block, l_f[1], l_f[0]);
  calc_hash_of_3D_block(hash_blocks_new_calc[2], 32, func_key_block, l_f_k[2], l_f_k[1], l_f_k[0]);
  calc_hash_of_2D_block(hash_blocks_new_calc[3], 32, key_block, l_k[1], l_k[0]);

  printf("Hash of nonce_block read in:\n");
  print_block_1B(hash_blocks[0], l_h[0]);
  printf("Hash of nonce_block the calculated value:\n");
  print_block_1B(hash_blocks_new_calc[0], l_h[0]);

  printf("\nHash of f_block read in:\n");
  print_block_1B(hash_blocks[1], l_h[0]);
  printf("Hash of nonce_block the calculated value:\n");
  print_block_1B(hash_blocks_new_calc[1], l_h[0]);

  printf("\nHash of func_key_block read in:\n");
  print_block_1B(hash_blocks[2], l_h[0]);
  printf("Hash of nonce_block the calculated value:\n");
  print_block_1B(hash_blocks_new_calc[2], l_h[0]);

  printf("\nHash of key_block read in:\n");
  print_block_1B(hash_blocks[3], l_h[0]);
  printf("Hash of nonce_block the calculated value:\n");
  print_block_1B(hash_blocks_new_calc[3], l_h[0]);

  printf("hash of nonce_block    : %s\n", (check_hash_block(hash_blocks[0], hash_blocks_new_calc[0], 32) ? "\x1B[32mis OK\x1B[0m" : "\x1B[31mis NOT OK!!!\x1B[0m"));
  printf("hash of f_block        : %s\n", (check_hash_block(hash_blocks[1], hash_blocks_new_calc[1], 32) ? "\x1B[32mis OK\x1B[0m" : "\x1B[31mis NOT OK!!!\x1B[0m"));
  printf("hash of func_key_block : %s\n", (check_hash_block(hash_blocks[2], hash_blocks_new_calc[2], 32) ? "\x1B[32mis OK\x1B[0m" : "\x1B[31mis NOT OK!!!\x1B[0m"));
  printf("hash of key_block      : %s\n", (check_hash_block(hash_blocks[3], hash_blocks_new_calc[3], 32) ? "\x1B[32mis OK\x1B[0m" : "\x1B[31mis NOT OK!!!\x1B[0m"));

  if (modus == 1) {
  printf("Now test encryption\n");
  encyrpt_file(filename_input, filename_output, nonce_block, f_block, func_key_block, key_block, exponent, rounds);
  } else if (modus == 2) {
    printf("Now test DEcryption\n");
    decyrpt_file(filename_input, filename_output, nonce_block, f_block, func_key_block, key_block, exponent, rounds);
  }
  FREE_ALL;
  printf("Finish all free!\n");
}

int isnumber(const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

int main(int argc, char* argv[]) {
  if (argc == 5) {
    if ((strcmp(argv[1], "--create-key") == 0) ||
        (strcmp(argv[1], "-c") == 0)) {
      printf("Is now kreating a new key\n");
    printf("argv[2] = %s\n", argv[2]);
    printf("argv[3] = %s\n", argv[3]);
    printf("argv[4] = %s\n", argv[4]);
      if (isnumber(argv[3]) && isnumber(argv[4])) {
        int exponent = atoi(argv[3]);
        int rounds = atoi(argv[4]);
        create_2B_ctr_key(argv[2], exponent, rounds);
      }
    } else if ((strcmp(argv[1], "--encrypt") == 0) ||
               (strcmp(argv[1], "-e") == 0)) {
      if ((access(argv[2], F_OK) != -1) && (access(argv[3], F_OK) != -1)) {
        crypt_program(1, argv[2], argv[3], argv[4]);
      }
    } else if ((strcmp(argv[1], "--decrypt") == 0) ||
               (strcmp(argv[1], "-d") == 0)) {
      if ((access(argv[2], F_OK) != -1) && (access(argv[3], F_OK) != -1)) {
        crypt_program(2, argv[2], argv[3], argv[4]);
      }
    }
  } else {
    printf("Invalid Input! Please give 5 parameters!\n");
    printf("possible program calls:\n");
    printf("./program (-c|--create-key) filename-key exponent rounds\n");
    printf("./program (-e|--encrypt) filename-key filename-input filename-output\n");
    printf("./program (-d|--decrypt) filename-key filename-input filename-output\n");
  }

  return 0;
}
