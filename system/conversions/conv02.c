/*
Note: itoa is a non-standard c function, while atoi, atoll and atof
are standard c functions


string to numeric decimal - atoi, atof, atol, atoll <stdlib.h>
-----------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
  char a[10] = "100";
  int value = atoi(a);
  printf("Value = %d\n", value);
  return 0;
  // Value = 100

  // --------------
  // string to float

  char b[10] = "3.14";
  float pi = atof(b);
  printf("Value of pi = %f\n", pi);
  return 0;
  // output Value of pi = 3.140000
}
