/*
 * Floating point literals in hexadecimal
 */
#include <stdio.h>

/* floating point literals in hexadecimal */
void printHexadecimal() {
  double d = 0x1.3p10; /* (1*(1/16) + 3*(1/16)) * (2^10)  == 1216*/
  printf("%f\n", d);

  // print hexadecimal version of a double
  double val = 3.1514234;
  printf("%a\n", val);
  printf("%A\n", val);
}

int main(void) {
  printHexadecimal();
  return 0;
}
