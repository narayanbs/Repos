/*
 * Strict aliasing rule in C standard enables the compiler to optimize code under the assumption
 * that pointers to different types do not point to the same memory location. This means that no two
 * pointers of incompatible types point to the same memory location.
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

void break_strict_aliasing() {
  float y = 3.1415;
  uint32_t l = *(uint32_t *)&y;
  l = 0x5f3759df - (l >> 1);
  y = *(float *)&l;
}

void valid_strict_aliasing() {
  float y = 3.1415;
  uint32_t l;
  assert(sizeof(l) == sizeof(y));
  memcpy(&l, &y, sizeof(l));
  l = 0x5f3759df - (l >> 1);
  memcpy(&y, &l, sizeof(y));
}

int main(void) {
  break_strict_aliasing();
  valid_strict_aliasing();
  return 0;
}
