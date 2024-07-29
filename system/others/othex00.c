/* floating point literals in hexadecimal */
#include <stdio.h>

int main(void) {
  double d = 0x0.3p10; /* 3*(16^-1)*(2^10)  == 192*/
  printf("%.f\n", d);
  return 0;
}
