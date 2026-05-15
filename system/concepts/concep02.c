/*
 * variadic function in C
 */
#include <stdarg.h>
#include <stdio.h>

/* variadic function in c */
void sum(int count, ...) {
  va_list(args);
  va_start(args, count);
  int sum = 0;
  for (int i = 0; i < count; i++) {
    sum += va_arg(args, int);
  }
  va_end(args);
  printf("The sum is %d\n", sum);
}

int main(void) {
  sum(3, 10, 20, 30);
  sum(4, 5, 10, 15, 20);
  return 0;
}
