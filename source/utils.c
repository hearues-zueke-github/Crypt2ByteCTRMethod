#include "../include/utils.h"

int rand_counter = 0;
int rand_old_clock = 0;

int readSizes(uint32* read_sizes, FILE* fsizes) {
  fseek(fsizes, 0, SEEK_SET);
  if (fsize(fsizes) != 32)
    return -1;

  fread(read_sizes, sizeof(uint32), 8, fsizes);

  uint32 max_sizes[8] = {128,              // comb_length
                         256 * 256 * 256,  // comb_size
                         256 * 256,        // ornt_amount
                         256 * 256 * 256,  // ornt_size
                         256 * 256,        // ornt_size_2
                         256 * 256,        // perm_amount
                         256 * 256,        // perm_size
                         256 * 256};       // block_size 
  int loop;
  for (loop = 0; loop < 8; loop++)
    if (read_sizes[loop] > max_sizes[loop])
      read_sizes[loop] = max_sizes[loop];

  return 0;
}

void init_srand() {
  rand_old_clock = clock();
  srand(rand_old_clock);
}

int randint(int min, int max) {
  if (rand_counter++ > 10000 && clock() != rand_old_clock)
  {
    rand_counter = 0;
    rand_old_clock = clock();
    srand(rand_old_clock);
  }
  if (min == max)
  {
    return min;
  }
  return rand() % (max - min) + min;
}

int fsize(FILE *fp) {
    int prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fseek(fp,prev,SEEK_SET); /*go back to where we were*/
    return sz;
}

// Print a Block of 256 Bytes size
void printBlock(uint8* block) {
  int loop;
  int loop2;
  printf("  %s", KGRN);
  for (loop = 0; loop < 0x10; loop++)
    printf(" 0%1X", loop);
  printf("\n");
  for (loop = 0; loop < 0x10; loop++) {
    printf("%s%s%X%s", KGRN, (loop == 0 ? "0" : ""), loop * 0x10, KNRM);
    for (loop2 = 0; loop2 < 0x10; loop2++)
      printf(" %s%1X", (block[loop * 0x10 + loop2] < 0x10 ? "0" : ""), block[loop * 0x10 + loop2]);
    printf("\n");
  }
}

void print_block_1B(uint8* block, size_t size) {
  int loop;
  int loop2;
  int split = 0x10;
  if (size <= 0x100) {
    printf("   %s", KGRN);
    for (loop = 0; loop < split; loop++)
      printf(" 0%1X", loop);
    printf("\n");
    int add_to_full_line = (size%split==0?0:1);
    for (loop = 0; loop < size / split + add_to_full_line; loop++) {
      printf("%s%s%X%s:", KGRN, (loop == 0 ? "0" : ""), loop * 0x10, KNRM);
      for (loop2 = 0; (loop2 < 0x10) && (loop * split + loop2 < size); loop2++)
        printf(" %s%1X", (block[loop * 0x10 + loop2] < 0x10 ? "0" : ""), block[loop * 0x10 + loop2]);
      printf("\n");
    }
  }
  else if (size <= 0x1000) {
    printf("    %s", KGRN);
    for (loop = 0; loop < split; loop++)
      printf(" 0%1X", loop);
    printf("\n");
    for (loop = 0; loop < size / split; loop++) {
      printf("%s%s%s%X%s:", KGRN,
                           (loop * 0x10 < 0x100 ? "0" : ""),
                           (loop * 0x10 < 0x10 ? "0" : ""),
                           loop * 0x10, KNRM);
      for (loop2 = 0; (loop2 < 0x10) && (loop * split + loop2 < size); loop2++)
        printf(" %s%1X", (block[loop * 0x10 + loop2] < 0x10 ? "0" : ""),
                             block[loop * 0x10 + loop2]);
      printf("\n");
    }
  }
  else if (size <= 0x10000) {
    printf("     %s", KGRN);
    for (loop = 0; loop < split; loop++)
      printf(" 0%1X", loop);
    printf("\n");
    for (loop = 0; loop < size / split; loop++) {
      printf("%s%s%s%s%X%s:", KGRN,
                           (loop * 0x10 < 0x1000 ? "0" : ""),
                           (loop * 0x10 < 0x100 ? "0" : ""),
                           (loop * 0x10 < 0x10 ? "0" : ""),
                           loop * 0x10, KNRM);
      for (loop2 = 0; (loop2 < 0x10) && (loop * split + loop2 < size); loop2++)
        printf(" %s%1X", (block[loop * 0x10 + loop2] < 0x10 ? "0" : ""),
                             block[loop * 0x10 + loop2]);
      printf("\n");
    }
  }
}

void print_block_2B(uint8* block, size_t size) {
  int i;
  int j;
  int split = 0x20;
  printf("     %s", KGRN);
  for (i = 0; i < split / 2; i++)
    printf(" %s%s%s%1X", (i < 0x1000 ? "0" : ""),
                         (i < 0x100  ? "0" : ""),
                         (i < 0x10   ? "0" : ""),
                         i);
  printf("\n");
  int add_to_full_line = (size%split==0?0:1);
  for (i = 0; i < size / split + add_to_full_line; i++) {
    printf("%s%s%s%s%X%s:", KGRN, (i * split / 2 < 0x1000 ? "0" : ""),
                                 (i * split / 2 < 0x100  ? "0" : ""),
                                 (i * split / 2 < 0x10   ? "0" : ""),
                                 i * split / 2, KNRM);
    for (j = 0; (j < split / 2) && (i * split + 2*j+1 < size); j++)
      printf(" %s%1X%s%1X", (block[i * split + 2*j+0] < 0x10 ? "0" : ""), block[i * split + 2*j+0],
                            (block[i * split + 2*j+1] < 0x10 ? "0" : ""),
                            block[i * split + 2*j+1]);
    if (add_to_full_line && ((size%split)%2==1) && (size-i*split<split))
      printf(" %s%1X", (block[i * split + size%split-1] < 0x10 ? "0" : ""), block[i * split + size%split-1]);
    printf("\n");
  }
}

int create_array(uint8* array, int l[], int dim) {
    int i = 0;
    uint8*** a = (uint8***)array;

    if (dim == 1) {
        if ((*(uint8**)array = (uint8*)malloc(l[dim - 1] * sizeof(uint8*))) == NULL)
            return -1;
        else
            return 0;
    }

    if ((*a = (uint8**)malloc(l[dim - 1] * sizeof(uint8*))) == NULL)
        return -1;

    for (i = 0; i < l[dim - 1]; i++) {
        uint8* temp;
        if (create_array((uint8*)&temp, l, dim - 1) == -1) {
            free_array((uint8*)*(uint8**)array, l, dim);
            return -1;
        }
        (*a)[i] = temp;
    }
  
    return 0;
}

void free_array(uint8* array, int l[], int dim) {
    int i = 0;

    uint8** a = (uint8**)array;

    if (dim == 1) {
        free((uint8*)a);
        return;
    }

    if (a == NULL)
        return;

    for (i = 0; i < l[dim - 1]; i++) {
        if (a[i] != NULL)
            free_array((uint8*)a[i], l, dim - 1);
        else
            break;
    }
    free(a);
}

int create_array_generic(void* array, int l[], int dim, size_t size) {
    int i = 0;
    void*** a = (void***)array;

    if (dim == 1) {
        if ((*(void**)array = (void*)malloc(l[dim - 1] * size)) == NULL)
            return -1;
        else
            return 0;
    }

    if ((*a = (void**)malloc(l[dim - 1] * sizeof(void*))) == NULL)
        return -1;

    for (i = 0; i < l[dim - 1]; i++) {
        void* temp;
        if (create_array((void*)&temp, l, dim - 1) == -1) {
            free_array_generic((void*)*(void**)array, l, dim);
            return -1;
        }
        (*a)[i] = temp;
    }
  
    return 0;
}

void free_array_generic(void* array, int l[], int dim) {
    int i = 0;

    void** a = (void**)array;

    if (dim == 1) {
        free((void*)a);
        return;
    }

    if (a == NULL)
        return;

    for (i = 0; i < l[dim - 1]; i++) {
        if (a[i] != NULL)
            free_array_generic((void*)a[i], l, dim - 1);
        else
            break;
    }
    free(a);
}

void define_random_1d_block(uint8* block, int length) {
  int i = 0;
  for (i = 0; i < length; i++)
    block[i] = randint(0, 255);
}

void define_random_2d_block(uint8** block, int l1, int l2) {
  int i = 0;
  for (i = 0; i < l1; i++)
    define_random_1d_block(block[i], l2);
}

void define_random_3d_block(uint8*** block, int l1, int l2, int l3) {
  int i = 0;
  for (i = 0; i < l1; i++)
    define_random_2d_block(block[i], l2, l3);
}

void define_f_Block_Generic(uint8** block, int m, int n) {
  int i = 0;
  int j = 0;
  int r1 = 0;
  int r2 = 0;
  int t = 0;
  // uint8 test_block[n];
  if (n <= 0x100) {
    for (j = 0; j < m; j++) {
      for (i = 0; i < n; i++) {
        block[j][i]   = i & 0xFF;
      }

      for (i = 0; i < n * 10; i++) {
        r1 = randint(0, n);
        while ((r2 = randint(0, n)) == r1) {}

        t = block[j][r1];
        block[j][r1] = block[j][r2];
        block[j][r2] = t;
      }
  // TODO: create a loop for testing!
      // setmem(test_block, 0, n);
//      for (i = 0; i < n; i)
    }
  }
  else {
    for (j = 0; j < m; j++) {
      for (i = 0; i < n; i++) {
        block[j][i*2]   = (i >> 8) & 0xFF;
        block[j][i*2+1] = i & 0xFF;
      }

      for (i = 0; i < n * 10; i++) {
        r1 = randint(0, n);
        while ((r2 = randint(0, n)) == r1) {}

        t = block[j][r1*2];
        block[j][r1*2] = block[j][r2*2];
        block[j][r2*2] = t;
        t = block[j][r1*2+1];
        block[j][r1*2+1] = block[j][r2*2+1];
        block[j][r2*2+1] = t;
      }
    }
  }
}

void define_fi_Block_Generic(uint8** f_block, uint8** fi_block, int m, int n) {
  int i;
  int j;
  int temp;

  if (n <= 0x100)
    for (j = 0; j < m; j++)
      for (i = 0; i < n; i++)
        fi_block[j][f_block[j][i]] = i;
  else
    for (j = 0; j < m; j++)
      for (i = 0; i < n; i++) {
        temp = ((f_block[j][2*i+0]<<8)+f_block[j][2*i+1])*2;
        fi_block[j][temp]   = (i>>8) & 0xFF;
        fi_block[j][temp+1] = i      & 0xFF;
        if (i<3&&j==0) {
          printf("f_block[j][i]              = 0x%2X 0x%2X\n", f_block[0][i*2], f_block[0][i*2+1]);
//          temp=((f_block[j][i*2+0]<<8)+f_block[j][i*2+1])*2;
          printf("temp = 0x%4X\n", temp/2);
          printf("fi_block[j][f_block[j][i]] = 0x%2X 0x%2X\n\n", fi_block[0][temp+0], fi_block[0][temp+1]);
        }
      }
  printf("before:\n");
//  print_block_2B(f_block[0], 0x200);
  print_block_2B(fi_block[0], 0x200);
//  printf("fd_block[0][0] = 0x%2X\n", fi_block[0][0]);
//  printf("fd_block[0][1] = 0x%2X\n", fi_block[0][1]);
}

void create_zero_byte_file(char* file_name, int length) {
  FILE* fout = fopen(file_name, "wb");
  uint8 zero_byte[] = {0};
  int i = 0;

  for (i = 0; i < length; i++)
    fwrite(zero_byte, 1, 1, fout);

  fclose(fout);
}

void xor_two_blocks(uint8* block1, uint8* block2, int length) {
  int i;
  for (i = 0; i < length; i++)
    block1[i] = block1[i] ^ block2[i];
}

void calc_hash_of_1D_block(uint8* hash_block, int hash_length, uint8* block_1D, int l1) {
  int i = 0;
  int x1;
  for (x1 = 0; x1 < l1; x1++)
    hash_block[(i++)%32] += block_1D[x1];
}
void calc_hash_of_2D_block(uint8* hash_block, int hash_length, uint8** block_2D, int l1, int l2) {
  int i = 0;
  int x1;
  int x2;
  for (x1 = 0; x1 < l1; x1++)
    for (x2 = 0; x2 < l2; x2++)
      hash_block[(i++)%32] += block_2D[x1][x2];
}
void calc_hash_of_3D_block(uint8* hash_block, int hash_length, uint8*** block_3D, int l1, int l2, int l3) {
  int i = 0;
  int x1;
  int x2;
  int x3;
  for (x1 = 0; x1 < l1; x1++)
    for (x2 = 0; x2 < l2; x2++)
      for (x3 = 0; x3 < l2; x3++)
        hash_block[(i++)%32] += block_3D[x1][x2][x3];
}

int check_hash_block(uint8* hash_block_1, uint8* hash_block_2, int size) {
  int i;
  for (i = 0; i < size; i++)
    if (hash_block_1[i] != hash_block_2[i])
      return 0;

  return 1;
}

void save_key_2B_generic(char* filename, uint8** fe_block, uint8** hash_values, uint8** fd_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds) {
  FILE* fout = fopen(filename, "wb");

  int key_iterations = 1<<(exponent-1);

  int l_h[] = {32, 4};
  int l_f[] = {0x20000, 0x100};
  int l_f_k[] = {key_iterations, exponent, 0x100};
  int l_k[] = {rounds, 1<<(32-exponent)};

  int i;
  int j;

  uint64 temp[1];
  temp[0] = exponent;
  fwrite(temp, sizeof(uint64), 1, fout);
  temp[0] = rounds;
  fwrite(temp, sizeof(uint64), 1, fout);

  for (i = 0; i < l_h[1]; i++)
    fwrite(hash_values[i], 1, l_h[0], fout);

  for (i = 0; i < l_f[1]; i++)
    fwrite(fe_block[i], 1, l_f[0], fout);

  for (i = 0; i < l_f[1]; i++)
    fwrite(fd_block[i], 1, l_f[0], fout);

  for (j = 0; j < l_f_k[2]; j++)
    for (i = 0; i < l_f_k[1]; i++)
      fwrite(func_key_block[j][i], 1, l_f_k[0], fout);

  for (i = 0; i < l_k[1]; i++)
    fwrite(key_block[i], 1, l_k[0], fout);

  fclose(fout);
}

void load_key_2B_generic(char* filename, uint8** fe_block, uint8** hash_values, uint8** fd_block, uint8*** func_key_block, uint8** key_block, int *exponent, int *rounds) {
  FILE* fin = fopen(filename, "rb");

//  printf("before exponent = %d\n", *exponent);
//  printf("before rounds   = %d\n", *rounds);

  uint64 temp[1];
  fread(temp, sizeof(uint64), 1, fin);
  *exponent = (int)temp[0];
  fread(temp, sizeof(uint64), 1, fin);
  *rounds = (int)temp[0];

//  printf("after exponent = %d\n", *exponent);
//  printf("after rounds   = %d\n", *rounds);

  int key_iterations = 1<<(*exponent-1);

  int l_h[] = {32, 4};
  int l_f[] = {0x20000, 0x100};
  int l_f_k[] = {key_iterations, *exponent, 0x100};
  int l_k[] = {*rounds, 1<<(32-*exponent)};

  int i;
  int j;

  for (i = 0; i < l_h[1]; i++)
    fread(hash_values[i], 1, l_h[0], fin);

  for (i = 0; i < l_f[1]; i++)
    fread(fe_block[i], 1, l_f[0], fin);

  for (i = 0; i < l_f[1]; i++)
    fread(fd_block[i], 1, l_f[0], fin);

  for (j = 0; j < l_f_k[2]; j++)
    for (i = 0; i < l_f_k[1]; i++)
      fread(func_key_block[j][i], 1, l_f_k[0], fin);

  for (i = 0; i < l_k[1]; i++)
    fread(key_block[i], 1, l_k[0], fin);

  fclose(fin);
}

void save_key_2B_ctr_generic(char* filename, uint8** hash_values, uint8* nonce_block, uint8** f_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds) {
  FILE* fout = fopen(filename, "wb");

  int key_iterations = 1<<(exponent-1);

  int l_h[] = {32, 4};
  int l_n[] = {1<<exponent};
  int l_f[] = {0x20000, 0x100};
  int l_f_k[] = {key_iterations, exponent, 0x100};
  int l_k[] = {rounds, 1<<(32-exponent)};

  int i;
  int j;

  uint64 temp[1];
  temp[0] = exponent;
  fwrite(temp, sizeof(uint64), 1, fout);
  temp[0] = rounds;
  fwrite(temp, sizeof(uint64), 1, fout);

  for (i = 0; i < l_h[1]; i++)
    fwrite(hash_values[i], 1, l_h[0], fout);

  fwrite(nonce_block, 1, l_n[0], fout);

  for (i = 0; i < l_f[1]; i++)
    fwrite(f_block[i], 1, l_f[0], fout);

  for (j = 0; j < l_f_k[2]; j++)
    for (i = 0; i < l_f_k[1]; i++)
      fwrite(func_key_block[j][i], 1, l_f_k[0], fout);

  for (i = 0; i < l_k[1]; i++)
    fwrite(key_block[i], 1, l_k[0], fout);

  fclose(fout);
}

void load_key_2B_ctr_generic(char* filename, uint8** hash_values, uint8* nonce_block, uint8** f_block, uint8*** func_key_block, uint8** key_block, int exponent, int rounds) {
  FILE* fin = fopen(filename, "rb");

  uint64 temp[1];
  fread(temp, sizeof(uint64), 1, fin);
  fread(temp, sizeof(uint64), 1, fin);

  int key_iterations = 1<<(exponent-1);

  int l_h[] = {32, 4};
  int l_n[] = {1<<exponent};
  int l_f[] = {0x20000, 0x100};
  int l_f_k[] = {key_iterations, exponent, 0x100};
  int l_k[] = {rounds, 1<<(32-exponent)};

  int i;
  int j;

  for (i = 0; i < l_h[1]; i++)
    fread(hash_values[i], 1, l_h[0], fin);

  fread(nonce_block, 1, l_n[0], fin);

  for (i = 0; i < l_f[1]; i++)
    fread(f_block[i], 1, l_f[0], fin);

  for (j = 0; j < l_f_k[2]; j++)
    for (i = 0; i < l_f_k[1]; i++)
      fread(func_key_block[j][i], 1, l_f_k[0], fin);

  for (i = 0; i < l_k[1]; i++)
    fread(key_block[i], 1, l_k[0], fin);

  fclose(fin);
}

void load_exp_round(char* filename, int* exponent, int* rounds) {
  FILE* fin = fopen(filename, "rb");

  uint64 temp[1];
  fread(temp, sizeof(uint64), 1, fin);
  *exponent = (int)temp[0];
  fread(temp, sizeof(uint64), 1, fin);
  *rounds = (int)temp[0];

  fclose(fin);
}
