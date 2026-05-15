/*
 * Floating point literals in hexadecimal
 */
#include <stdio.h>

/* floating point literals in hexadecimal */
void printHexadecimal() {
  double d = 0x0.3p10; /* 3*(16^-1)*(2^10)  == 192*/
  printf("%.f\n", d);
}

int main(void) {
  printHexadecimal();
  return 0;
}
