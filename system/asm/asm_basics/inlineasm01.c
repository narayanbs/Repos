#include <stdio.h>

int main(void) {
  int src = 8;
  int dst;
  asm("shrl $1, %0" : "=r"(dst) : "r"(src) : "cc");
  printf("%d\n", dst);
  return 0;
}
