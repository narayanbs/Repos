/*
Numeric to string - snprintf
--------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
  int a = 234234;
  int len = snprintf(NULL, 0, "%d", a);
  char buf[len + 1];
  snprintf(buf, len + 1, "%d", a);
  printf("%s\n", buf);

  float amount = 2.12314;
  len = snprintf(NULL, 0, "%f", amount);
  printf("%d ", len);
  char *result = malloc(len + 1);
  snprintf(result, len + 1, "%f", amount);
  printf("%s\n", result);
  free(result);
}
