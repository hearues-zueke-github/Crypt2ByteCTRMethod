#include "../include/check_key_2B_1k.h"

int check_f_block(uint8** block) {
  uint8 count;
  int index;
  int i;
  int j;

  if (BLOCK_F_N <= 0x100) {
    uint8 block_count[BLOCK_F_N];
    memset(block_count, 0, BLOCK_F_N);
    for (j = 0; j < BLOCK_F_M; j++) {
      for (i = 0; i < BLOCK_F_N; i++) {
        index = (int)block[j][i];
        if (block_count[index] == count)
          block_count[index]++;
        else
          return -1;
      }
      count++;
    }
  }
  else {
    uint8 block_count[BLOCK_F_N * 2];
    memset(block_count, 0, BLOCK_F_N * 2);
    for (j = 0; j < BLOCK_F_M; j++) {
      for (i = 0; i < BLOCK_F_N; i++) {
        index = (int)(block[j][2*i] << 8) + (int)block[j][2*i+1];
        if (block_count[index] == count)
          block_count[index]++;
        else
          return -1;
      }
      count++;
    }
  }

  return 0;
}

int check_f_fp_block(uint8** block_f, uint8** block_fp) {
  int i;
  int j;
  int index;
  for (j = 0; j < BLOCK_F_M; j++)
    for (i = 0; i < BLOCK_F_N; i++) {
      index = ((int)block_f[j][2*i] << 8) + (int)block_f[j][2*i+1];
      if (i != ((int)block_fp[j][2*index] << 8) + (int)block_fp[j][2*index+1])
        return -1;
    }

  return 0;
}

int main(int argc, char* argv[]) {
  // Init the randomizer
  init_srand();

  // Define all needed blocks
  uint8**  block_f;
  uint8**  block_fp;
  uint8*** block_o;
  uint8**  block_c;
  uint8* whole_block;

  // Set the foc Blocks directly with the new recursive methods create_array
  int l_whole_block[] = {MAX_MEM};

  CREATE_ARRAY
  create_array((uint8*)&whole_block, l_whole_block, 1);

  // Test, if malloc was successful
  if (MALLOC_FAIL || (whole_block == NULL)) {
    FREE_MALLOC
    if (whole_block != NULL)
      free_array(whole_block, l_whole_block, 1);
    printf("Error: Not possible to allocate Memory!\nTerminating program!\n");
    return -1;
  }

  FILE* fout = fopen("/home/haris/Documents/key_2B_1k.byte", "r");
  fread(whole_block, sizeof(uint8), MAX_MEM, fout);
  fclose(fout);

  get_foc_Block(whole_block, block_f, block_fp, block_o, block_c);  

  if (PRINT_BLOCKS) {
    printf("block_f_n = %d\n", BLOCK_F_N);
    if (BLOCK_F_N <= 0x100) {
      printBlock(block_f[0]);
      printBlock(block_fp[0]);
    }
    else {
      print_block_2B(block_f[0], BLOCK_F_N * 2);
      print_block_2B(block_fp[0], BLOCK_F_N * 2);
    }
  }

  uint8* b_f = block_f[0];
  uint8* b_fp = block_fp[0];
  int i;
  i = 0;
  if (BLOCK_F_N <= 0x100) {
    printf("b_f[0x%2X] = 0x%2X\n", i, (int)b_f[i]);
    i = (int)b_f[i];
    printf("b_fp[0x%2X] = 0x%2X\n", i, (int)b_fp[i]);
  }
  else {
    printf("b_f[0x%2X] = 0x%2X\n", i, ((int)b_f[2*i] << 8) + b_f[2*i+1]);
    i = ((int)b_f[2*i] << 8) + b_f[2*i+1];
    printf("b_fp[0x%2X] = 0x%2X\n", i, ((int)b_fp[2*i] << 8) + b_fp[2*i+1]);
  }

  printf("Checking f and fp Block:\n");
  printf("check block_f: %s\n", (check_f_block(block_f) == 0 ? "is OK" : "is NOT OK, something is wrong"));
  printf("check block_fp: %s\n", (check_f_block(block_fp) == 0 ? "is OK" : "is NOT OK, something is wrong"));
  printf("check block_f and block_fp simulatnious: %s\n", (check_f_fp_block(block_f, block_fp) == 0 ? "is OK" : "is NOT OK, something is wrong"));

  FREE_MALLOC
  if (whole_block != NULL)
    free_array(whole_block, l_whole_block, 1);

  return 0;
}
