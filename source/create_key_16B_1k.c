#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

struct stat st = {0};

int main(int argc, char* argv[])
{
  
}

int old_key_256_bytes_block()
{
  rand_old_clock = clock();
  srand(rand_old_clock);

  if (stat("keys_hz_method", &st) == -1)
    mkdir("keys_hz_method", 0700);

  FILE* file_complete_combination = fopen("keys_hz_method/combination_key.byte", "wb");

  uint32 all_sizes[8] = {8,               // comb_length
                         256 * 256 * 256, // comb_size
                         256,             // ornt_amount
                         256 * 256,       // ornt_size
                         256,             // ornt_size_2
                         256,             // perm_amount
                         256,             // perm_size
                         256};            // block_size

  FILE* fs = fopen("keys_hz_method/sizes.byte", "wb");
  fwrite(all_sizes, sizeof(uint32), 8, fs);
  fclose(fs);

  int size = 0;
  int i = 0;
  int loop = 0;

  /* Key for Combination */

  size = 8 * 256 * 256 * 256;
  char* temp_block3 = (char*)malloc(sizeof(char) * size);
  if (temp_block3 == NULL)
  {
    printf("Error Malloc!\n");
    return -1;
  }

  for (i = 0; i < size; i++)
  {
    temp_block3[i] = randint(0, 256);
  }

  fwrite(temp_block3, 1, size, file_complete_combination);

  free(temp_block3);

  // Malloc 256 Bytes
  char* temp_block = (char*)malloc(sizeof(char) * 256);
  if (temp_block == NULL)
  {
    printf("Error Malloc!\n");
    return -1;
  }

  // Create 0 ... 256 Numbers
  for (i = 0; i < 256; i++)
    temp_block[i] = i;

  int t;
  int r1;
  int r2;

  /* Key for Orientation */

  int key = 0;

  for (key = 0; key < 256; key++)
  {
    for (loop = 0; loop < 256; loop++)
    {
      // Mix Array
      for (i = 0; i < 256; i++)
      {
        r1 = randint(0, 256);
        while ((r2 = randint(0, 256)) == r1) {}
        
        t = temp_block[r1];
        temp_block[r1] = temp_block[r2];
        temp_block[r2] = t;
      }

      fwrite(temp_block, 1, 256, file_complete_combination);
    }
  }

  /* Keys for Permutation */

  int limit = 256;

  unsigned char* temp_block2 = (unsigned char*)malloc(sizeof(unsigned char) * limit);

  if (temp_block2 == NULL)
  {
    printf("Error Malloc!\n");
    return -1;
  }

  int count = 0;

  for (key = 0; key < 256; key++)
  {
    count = 0;

    for (loop = 0; loop < limit; loop++)
      temp_block2[loop] = (loop + 1) % limit;

    while (count != limit)
    {
      for (loop = 0; loop < limit; loop++)
      {
        r1 = randint(0, limit);
        while (((r2 = randint(0, limit)) == r1) || (r1 == temp_block2[r2]) || (r2 == temp_block2[r1])) {}
        t = temp_block2[r1];
        temp_block2[r1] = temp_block2[r2];
        temp_block2[r2] = t;
      }

      t = temp_block2[0];
      count = 0;
      while (1)
      {
        count++;
        if (t == 0)
          break;
        t = temp_block2[t];
      }
    }

    t = temp_block2[0];

    count = 0;
    while (1)
    {
      count++;
      if (t == 0)
        break;
      t = temp_block2[t];
    }

    fwrite(temp_block2, sizeof(unsigned char), limit, file_complete_combination);
  }

  free(temp_block2);
  free(temp_block);
  fclose(file_complete_combination);

  return 0;
}
