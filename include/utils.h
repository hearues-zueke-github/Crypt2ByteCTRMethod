#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define true 1
#define false 0

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

// Read 8 uint32 numbers from a file
int readSizes(uint32* read_sizes, FILE* fsizes);

// Init the randomizer
void init_srand();

// Get a number between include min and exlude max
int randint(int min, int max);

// Get file size
int fsize(FILE *fp);

// Print a Block of 256 Bytes size
void printBlock(uint8* block);

// Print a Block with 1 Bytes
void print_block_1B(uint8* block, size_t size);

// Print a Block of 1024 Bytes size (but two bytes together)
void print_block_2B(uint8* block, size_t size);

// Creates an block of array with random data
void define_random_1d_block(uint8* block, int length);
void define_random_2d_block(uint8** block, int l1, int l2);
void define_random_3d_block(uint8*** block, int l1, int l2, int l3);

// 2D Block of m x f Functions with size n
// possible sizes for m: 1 to 2^8 or even too 2^16
// (better lower than 2^16, and something with 2^n)
// possible sizes for n: 1 to 2^8 or even too 2^16
// for 1 Byte or 2 Byte sizes
// To get the size of block:
// if n <  0x100 (2^8): size = m * n * 1 (because of ONE byte used per Number)
// if n >= 0x100 (2^8): size = m * n * 2 (because of TWO byte used per Numbblock, int hash_length
void define_f_Block_Generic(uint8** block, int m, int n);

void define_fi_Block_Generic(uint8** f_block, uint8** fi_block, int m, int n);

void create_zero_byte_file(char* file_name, int length);

void xor_two_blocks(uint8* block1, uint8* block2, int length);

void calc_hash_of_1D_block(uint8* hash_block, int hash_length, uint8* block_1D, int l1);
void calc_hash_of_2D_block(uint8* hash_block, int hash_length, uint8** block_2D, int l1, int l2);
void calc_hash_of_3D_block(uint8* hash_block, int hash_length, uint8*** block_3D, int l1, int l2, int l3);

int check_hash_block(uint8* hash_block_1, uint8* hash_block_2, int size);

void save_key_2B_generic(char* filename, uint8** fe_block, uint8** hash_values, uint8** fd_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds);
void load_key_2B_generic(char* filename, uint8** fe_block, uint8** hash_values, uint8** fd_block, uint8*** func_key_block, uint8** key_block, int *exponent, int *rounds);

void save_key_2B_ctr_generic(char* filename, uint8** hash_values, uint8* nonce_block, uint8** f_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds);
void load_key_2B_ctr_generic(char* filename, uint8** hash_values, uint8* nonce_block, uint8** f_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds);
void load_exp_round(char* filename, int* exponent, int* rounds);

// Somehow n dimensional array with type uint8 (unsigned char)
int create_array(uint8* array, int l[], int dim);

// Free the whole dynamic initialized array of create_array (with type uint8)
void free_array(uint8* array, int l[], int dim);

// Somehow n dimensional array with type void* (generic)
int create_array_generic(void* array, int l[], int dim, size_t size);

// Free the whole dynamic initialized array of create_array (with type void*, generic)
void free_array_generic(void* array, int l[], int dim);

#endif /* UTILS_H_INCLUDED */
