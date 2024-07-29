#include <stdint.h>
#include <stdio.h>

uint64_t factorial(uint64_t);

int main() {
  for (uint64_t i = 0; i < 20; i++) {
    printf("factorial(%2lu) = %lu\n", i, factorial(i));
  }
  return 0;
}
