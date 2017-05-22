#ifndef UTILS_H

#include <stdio.h>
#include <stdint.h>

#define AC_RED_BOLD "\x1b[1;49;31m"

#define AC_RED     "\x1b[31m"
#define AC_GREEN   "\x1b[32m"
#define AC_YELLOW  "\x1b[33m"
#define AC_BLUE    "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN    "\x1b[36m"
#define AC_D_W     "\x1b[37m"
#define AC_B_W     "\x1b[1;37m"
#define AC_RESET   "\x1b[0m"

void printBlock(uint8_t* block, int size, int width);

#endif /* UTILS_H */
