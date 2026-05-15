/*
 * Print individual bytes to show endianess of
 * the system
 */
#include <stdio.h>

int main(void) {
  int x = 0x12345678;
  unsigned char *p = (unsigned char *)&x;
  for (int i = 0; i < sizeof(int); i++) {
    printf("%d = 0x%02X\n", i, p[i]);
  }
  return 0;
}
