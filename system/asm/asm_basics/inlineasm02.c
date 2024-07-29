#include <stdio.h>

int main(void) {
  int src = 8;
  int dst;
  asm("mov %1, %0\n\t"
      "add $1, %0"
      : "=r"(dst)
      : "r"(src));

  printf("%d\n", dst);
  return 0;
}
