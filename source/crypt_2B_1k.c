#include "utils.h"
#include "crypt_2B_1k.h"

int l_f[] = {(BLOCK_F_N <= 0x100 ? BLOCK_F_N : BLOCK_F_N * 2), BLOCK_F_M};
int l_o[] = {(BLOCK_F_N <= 0x100 ? BLOCK : BLOCK / 2), BLOCK_O_I * 2, BLOCK_O_B};
int l_c[] = {BLOCK_C_I, BLOCK_C_N};

void get_foc_Block(uint8* whole_block, uint8** f_block, uint8** fp_block, uint8*** o_block, uint8** c_block) {
  int i = 0;
  int j = 0;
  int k = 0;
  int off = 0;
  if (BLOCK_F_N <= 0x100) {
    for (j = 0; j < BLOCK_F_M; j++) {
      for (i = 0; i < BLOCK_F_N; i++) {
        f_block[j][i] = whole_block[off];
        off++;
      }
    }

    for (j = 0; j < BLOCK_F_M; j++) {
      for (i = 0; i < BLOCK_F_N; i++) {
        fp_block[j][i] = whole_block[off];
        off++;
      }
    }

    for (k = 0; k < BLOCK_O_B; k++) {
      for (j = 0; j < BLOCK_O_I * 2; j++) {
        for (i = 0; i < BLOCK; i++) {
          o_block[k][j][i] = whole_block[off];
          off++;
        }
      }
    }
  }
  else {
    for (j = 0; j < BLOCK_F_M; j++) {
      for (i = 0; i < BLOCK_F_N * 2; i++) {
        f_block[j][i] = whole_block[off];
        off++;
      }
    }

    for (j = 0; j < BLOCK_F_M; j++) {
      for (i = 0; i < BLOCK_F_N * 2; i++) {
        fp_block[j][i] = whole_block[off];
        off++;
      }
    }

    for (k = 0; k < BLOCK_O_B; k++) {
      for (j = 0; j < BLOCK_O_I * 2; j++) {
        for (i = 0; i < BLOCK / 2; i++) {
          o_block[k][j][i] = whole_block[off];
          off++;
        }
      }
    }
  }
  for (j = 0; j < BLOCK_C_N; j++) {
    for (i = 0; i < BLOCK_C_I; i++) {
      c_block[j][i] = whole_block[off];
      off++;
    }
  }
}
