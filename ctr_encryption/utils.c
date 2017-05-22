#include "utils.h"

void printBlock(uint8_t* block, int size, int width) {
  // prepare first row for numbering
  printf("        ");
  for (int loop = 0; loop < width; loop++) {
    printf(AC_GREEN " %02X" AC_RESET, loop);
  }

  printf("\n");

  for (int loop = 0; loop < size; loop++) {
    if (loop % width == 0) {
      printf(AC_YELLOW "0x%05X:" AC_RESET, loop);
    }

    if (loop % 2 == 0) {
      printf(AC_D_W);
    } else {
      printf(AC_B_W);
    }

    printf(" %02X", block[loop] & 0xFF);
    printf(AC_RESET);

    if ((loop != width) && ((loop%width) == (width-1))) {
      printf("\n");
    }
  }
  if ((size%width != 0)) {
    printf("\n");
  }
}
