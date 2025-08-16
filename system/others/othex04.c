#include <complex.h>
#include <stdio.h>

int main(void) {
  double complex x = CMPLX(2.0, 3.0);
  double complex y = 3.0 + 2.0I;
  double complex z = x + y;
  printf("z = %.1f + %.1fi", creal(z), cimag(z));
  double w = x + y;
  // only real part, imaginary part is ignored
  printf("z = %.1f + %.1fi", creal(z), cimag(z));
  return 0;
}
