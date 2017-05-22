#include <stdio.h>

int num(int a) {
  printf("a = %d\n", a);
  return a;
}

// bool less(int a, int b) { return a < b; }

int main(int argc, char* argv[]) {
  int a = 5;
  for (int i = 0; i < a; i++) {
    printf("i = %d\n", i);
  }

  int b[] = {1,2,3};
  printf("b[1]   = %d\n", b[1]);
  printf("1[b]   = %d\n", 1[b]);
  printf("*(b+1) = %d\n", *(b+1));
  printf("Hello, World!\n");
  return 0;
}
